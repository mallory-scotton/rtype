///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Network/FFragmentManager.hpp>
#include <algorithm>
#include <Engine/Network/Packets.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
void FragmentManager::Update(Float32 deltaTime, FNetworkBase* networkBase)
{
    const UInt32 currentTime = static_cast<UInt32>(deltaTime);

    for (auto& entry: m_fragments)
    {
        if (entry.destination == EFragmentDestination::Incoming &&
            IsFragmentComplete(entry))
        {
            MergeFragments(entry, networkBase);
            MarkFragmentForDeletion(entry.id);
            continue;
        }
        if (currentTime - entry.timestamp >= DELETION_TIMEOUT)
        {
            MarkFragmentForDeletion(entry.id);
            continue;
        }
        if (entry.destination == EFragmentDestination::Outgoing)
        {
            SendRetransmission(entry, networkBase);
        }
    }

    DestroyFragments();
}

///////////////////////////////////////////////////////////////////////////////
void FragmentManager::SortFragments(FragmentEntry& entry)
{
    std::sort(
        entry.chunks.begin(),
        entry.chunks.end(),
        [](const Chunk& a, const Chunk& b)
        { return a.sequenceOrder < b.sequenceOrder; }
    );
}

///////////////////////////////////////////////////////////////////////////////
void FragmentManager::DestroyFragments(void)
{
    if (m_fragmentsToDelete.empty()) { return; }

    std::cout << "[manager] we deleting and shit" << std::endl;
    m_fragments.erase(
        std::remove_if(
            m_fragments.begin(),
            m_fragments.end(),
            [this](const FragmentEntry& entry)
            {
                return std::find(
                           m_fragmentsToDelete.begin(),
                           m_fragmentsToDelete.end(),
                           entry.id
                       ) != m_fragmentsToDelete.end();
            }
        ),
        m_fragments.end()
    );

    m_fragmentsToDelete.clear();
}

///////////////////////////////////////////////////////////////////////////////
void FragmentManager::MarkFragmentForDeletion(const UUID& fragmentID)
{
    if (std::find(
            m_fragmentsToDelete.begin(), m_fragmentsToDelete.end(), fragmentID
        ) == m_fragmentsToDelete.end())
    {
        m_fragmentsToDelete.push_back(fragmentID);
    }
}

///////////////////////////////////////////////////////////////////////////////
void FragmentManager::MergeFragments(
    FragmentEntry& entry, FNetworkBase* networkBase
)
{
    if (!networkBase) { return; }

    // Ensure fragments are sorted by sequence order
    SortFragments(entry);

    // Reserve space for the complete data
    std::vector<Byte> completeData;
    SizeT totalSize = 0;
    for (const auto& chunk: entry.chunks) { totalSize += chunk.data.size(); }
    completeData.reserve(totalSize);

    // Merge all chunks in order
    for (const auto& chunk: entry.chunks)
    {
        completeData.insert(
            completeData.end(), chunk.data.begin(), chunk.data.end()
        );
    }
    std::cout << "[manager] its processing time little one" << std::endl;
    // Process the reassembled packet through the normal packet pipeline
    networkBase->ProcessReceivedData(
        completeData.data(), completeData.size(), entry.sender
    );

    // Mark as fully received
    entry.fullyReceived = true;
}

///////////////////////////////////////////////////////////////////////////////
Bool FragmentManager::IsFragmentComplete(const FragmentEntry& entry) const
{
    if (entry.chunks.size() != entry.chunkCount) { return false; }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<std::vector<Byte>>
    FragmentManager::FragmentPacket(const std::vector<Byte>& serializedData
    ) const
{
    std::vector<std::vector<Byte>> chunks;

    // Calculate number of chunks needed
    const SizeT totalSize = serializedData.size();
    const SizeT numChunks =
        (totalSize + MAX_FRAGMENT_SIZE - 1) / MAX_FRAGMENT_SIZE;

    chunks.reserve(numChunks);

    // Split into chunks
    for (SizeT i = 0; i < totalSize; i += MAX_FRAGMENT_SIZE)
    {
        const SizeT chunkSize = std::min(MAX_FRAGMENT_SIZE, totalSize - i);
        std::vector<Byte> chunk(
            serializedData.begin() + i, serializedData.begin() + i + chunkSize
        );
        chunks.push_back(std::move(chunk));
    }

    return chunks;
}

///////////////////////////////////////////////////////////////////////////////
FragmentEntry* FragmentManager::FindFragmentEntry(const UUID& fragmentID)
{
    for (auto& entry: m_fragments)
    {
        if (entry.id == fragmentID) { return &entry; }
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
const FragmentEntry* FragmentManager::FindFragmentEntry(const UUID& fragmentID
) const
{
    for (const auto& entry: m_fragments)
    {
        if (entry.id == fragmentID) { return &entry; }
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
UInt32 FragmentManager::GetCurrentTimestamp(void) const
{
    auto now = SteadyClock::now();
    auto epoch = now.time_since_epoch();
    return static_cast<UInt32>(
        std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count()
    );
}

///////////////////////////////////////////////////////////////////////////////
UUID FragmentManager::SendFullTransmission(
    const std::vector<Byte>& serializedData,
    const std::vector<FEndpoint>& destinations,
    FNetworkBase* networkBase
)
{
    if (!networkBase) { return UUID::Nil; }
    std::cout << "[MANAGER] we out here and shit" << std::endl;
    // Create new fragment entry
    FragmentEntry entry;
    entry.id = UUID::V4();   // Generate unique ID
    entry.timestamp = GetCurrentTimestamp();
    entry.destination = EFragmentDestination::Outgoing;
    entry.destinations = destinations;
    entry.original = serializedData;   // Store original data for reference

    // Fragment the packet
    auto chunks = FragmentPacket(serializedData);
    entry.chunkCount = chunks.size();

    // Convert UUID to PackageID (first 4 bytes)
    const auto& uuidData = entry.id.Data();
    UInt32 packageID = (static_cast<UInt32>(uuidData[0]) << 24) |
                       (static_cast<UInt32>(uuidData[1]) << 16) |
                       (static_cast<UInt32>(uuidData[2]) << 8) |
                       static_cast<UInt32>(uuidData[3]);
    std::cout << "[MANAGER] id created" << std::endl;

    // Create chunk entries and send them
    for (SizeT i = 0; i < chunks.size(); ++i)
    {
        Chunk chunk;
        chunk.sequenceOrder = i;
        chunk.data = std::move(chunks[i]);

        // Initialize status for each destination
        for (const auto& dest: destinations)
        {
            Chunk::Status status;
            status.received = false;
            status.lastSentTimestamp = GetCurrentTimestamp();
            chunk.statuses[dest] = status;
        }

        entry.chunks.push_back(std::move(chunk));

        // Send this chunk to all destinations
        Packets::Fragment fragmentPacket;
        fragmentPacket.PackageID = packageID;
        fragmentPacket.SequenceID = static_cast<UInt8>(i);
        fragmentPacket.FragmentCount = static_cast<UInt8>(entry.chunkCount);
        fragmentPacket.data = entry.chunks[i].data;

        FLogger::SetNamespace("Fragment");
        FLogger::Info("transmitting chunk {} ...", i);
        for (const auto& dest: destinations)
        {
            networkBase->SendPacket(fragmentPacket, dest);
        }
    }

    // Store the entry
    m_fragments.push_back(std::move(entry));

    return entry.id;
}

///////////////////////////////////////////////////////////////////////////////
void FragmentManager::SendRetransmission(
    const FragmentEntry& entry, FNetworkBase* networkBase
)
{
    // Only retransmit outgoing fragments
    if (entry.destination != EFragmentDestination::Outgoing || !networkBase)
    {
        return;
    }

    const UInt32 currentTime = GetCurrentTimestamp();

    // Check each destination for missing chunks
    for (const auto& destination: entry.destinations)
    {
        for (const auto& chunk: entry.chunks)
        {
            // Find status for this destination
            auto statusIt = chunk.statuses.find(destination);
            if (statusIt == chunk.statuses.end()) { continue; }

            const auto& status = statusIt->second;

            // If not received and enough time has passed since last send
            if (!status.received &&
                (currentTime - status.lastSentTimestamp) >= ACK_TIMEOUT)
            {
                // Create Fragment packet for this chunk
                Packets::Fragment fragmentPacket;

                const auto& uuidData = entry.id.Data();
                fragmentPacket.PackageID =
                    (static_cast<UInt32>(uuidData[0]) << 24) |
                    (static_cast<UInt32>(uuidData[1]) << 16) |
                    (static_cast<UInt32>(uuidData[2]) << 8) |
                    static_cast<UInt32>(uuidData[3]);
                fragmentPacket.SequenceID =
                    static_cast<UInt8>(chunk.sequenceOrder);
                fragmentPacket.FragmentCount =
                    static_cast<UInt8>(entry.chunkCount);
                fragmentPacket.data = chunk.data;

                networkBase->SendPacket(fragmentPacket, destination);

                // Update last sent timestamp
                const_cast<Chunk::Status&>(status).lastSentTimestamp =
                    currentTime;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void FragmentManager::ProcessFragment(
    const IPacket& packet, const FEndpoint& sender, FNetworkBase* networkBase
)
{
    // Cast to Fragment packet
    const auto* fragmentPacket =
        dynamic_cast<const Packets::Fragment*>(&packet);
    if (!fragmentPacket || !networkBase) { return; }

    // Find or create fragment entry
    UUID fragmentID =
        UUID::Fill(static_cast<UInt8>(fragmentPacket->PackageID));
    FragmentEntry* entry = FindFragmentEntry(fragmentID);
    FLogger::SetNamespace("Fragment manager");
    FLogger::Info("bew fragment received");
    if (!entry)
    {
        // Create new incoming fragment entry
        FragmentEntry newEntry;
        newEntry.id = fragmentID;
        newEntry.timestamp = GetCurrentTimestamp();
        newEntry.destination = EFragmentDestination::Incoming;
        newEntry.chunkCount = fragmentPacket->FragmentCount;
        newEntry.chunks.resize(fragmentPacket->FragmentCount);
        newEntry.sender = sender;   // Store sender for reassembly

        m_fragments.push_back(std::move(newEntry));
        entry = &m_fragments.back();
    }

    // Store the chunk data
    if (fragmentPacket->SequenceID < entry->chunks.size())
    {
        Chunk& chunk = entry->chunks[fragmentPacket->SequenceID];
        chunk.sequenceOrder = fragmentPacket->SequenceID;
        chunk.data = fragmentPacket->data;

        // Mark as received from this sender
        Chunk::Status status;
        status.received = true;
        status.lastSentTimestamp = 0;
        chunk.statuses[sender] = status;

        // Send acknowledgment back to sender
        Packets::FragmentAcknowledgment ack;
        ack.PackageID = fragmentPacket->PackageID;
        ack.FragmentID = fragmentPacket->SequenceID;
        networkBase->SendPacket(ack, sender);
    }
}

}   // namespace tkd
