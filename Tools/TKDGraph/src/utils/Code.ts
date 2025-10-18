/** Dependencies */
import type { BlueprintData } from '../types';
import { NodeRegistry } from './NodeRegistry';
import { convertPinTypeToDefaultValue, convertPinTypeToEngineType } from './Convert';

function normalizeIds(id: string): string {
  // Replace all invalid characters with underscores and convert to uppercase
  return id.replace(/[^a-zA-Z0-9_]/g, '_').toUpperCase();
}

export function generateCodeFromEvent(blueprint: BlueprintData, eventId: string, registry: NodeRegistry): string {
  // Map to hold node snippets
  const nodeSnippets = new Map<string, string>();

  // Preload snippets for all nodes
  for (const node of blueprint.nodes) {
    const template = registry.getTemplate(node.template);
    if (!template) {
      continue;
    }
    nodeSnippets.set(node.data.id, template.snippet);
  }

  // Find the event node
  const eventNode = blueprint.nodes.find((node) => node.data.type === 'event' && node.template === eventId);

  // If event node not found, return empty string
  if (!eventNode) {
    console.warn(`Event node "${eventId}" not found.`);
    return '';
  }

  // Recursive function to build code from nodes
  let visited = new Set<string>();

  function buildCodeFromNode(nodeId: string): string {
    if (visited.has(nodeId)) {
      return ''; // Prevent cycles
    }
    visited.add(nodeId);

    const node = blueprint.nodes.find((n) => n.data.id === nodeId);
    if (!node) {
      return '';
    }

    // Get node snippet
    let snippet = nodeSnippets.get(nodeId);

    // If no snippet, skip this node
    if (!snippet) {
      return '';
    }

    // Process input connections first
    node.data.inputs?.forEach((inputPin, inputIdx) => {
      // Get the placeholder for this input
      const placeholder = `{INPUT_${inputIdx + 1}}`;

      // Check if the snippet includes this placeholder
      if (!snippet?.includes(placeholder)) {
        return;
      }

      // Find connected node
      const connectedAsSourcePin = blueprint.connections.find(
        (conn) => conn.sourceNodeId === nodeId && conn.sourcePinId === inputPin.id
      );
      const connectedAsTargetPin = blueprint.connections.find(
        (conn) => conn.targetNodeId === nodeId && conn.targetPinId === inputPin.id
      );

      // Check if connectedPin is source or target
      const connectedPin = connectedAsSourcePin || connectedAsTargetPin;

      // If not connected, skip
      if (!connectedPin) {
        let value = inputPin.value || convertPinTypeToDefaultValue(inputPin.type);
        let symbol = convertPinTypeToEngineType(inputPin.type);

        snippet = snippet.replace(placeholder, value === null ? 'nullptr' : `${symbol}(${value.toString()})`);
        return;
      }

      // Check pin type
      if (inputPin.type === 'exec') {
        //? INFO: The exec input is handled recursively by the output processing
        snippet = snippet.replace(placeholder, '');
      } else {
        const connectedNodeId =
          connectedPin.targetNodeId === nodeId ? connectedPin.sourceNodeId : connectedPin.targetNodeId;
        const code = buildCodeFromNode(connectedNodeId);

        snippet = code + '\n' + snippet;

        snippet = snippet.replace(
          placeholder,
          normalizeIds(connectedPin.targetNodeId === nodeId ? connectedPin.sourcePinId : connectedPin.targetPinId)
        );
      }
    });

    // Process output connections
    node.data.outputs?.forEach((outputPin, outputIdx) => {
      // Get the placeholder for this output
      const placeholder = `{OUTPUT_${outputIdx + 1}}`;

      // Check if the snippet includes this placeholder
      if (!snippet?.includes(placeholder)) {
        return;
      }

      // Find connected node
      const connectedAsSourcePin = blueprint.connections.find(
        (conn) => conn.sourceNodeId === nodeId && conn.sourcePinId === outputPin.id
      );
      const connectedAsTargetPin = blueprint.connections.find(
        (conn) => conn.targetNodeId === nodeId && conn.targetPinId === outputPin.id
      );

      // Check if connectedPin is source or target
      const connectedPin = connectedAsSourcePin || connectedAsTargetPin;

      // If not connected, skip
      if (!connectedPin) {
        snippet = snippet.replace(placeholder, '');
        return;
      }

      if (outputPin.type === 'exec') {
        snippet = snippet.replace(
          placeholder,
          buildCodeFromNode(
            connectedPin.targetNodeId === nodeId ? connectedPin.sourceNodeId : connectedPin.targetNodeId
          )
        );
      } else {
        snippet = snippet.replace(placeholder, normalizeIds(outputPin.id));
      }
    });

    return snippet;
  }

  return buildCodeFromNode(eventNode.data.id);
}
