/** Dependencies */
import { useState, useEffect, useRef, type ReactElement } from 'react';
import type { BlueprintData, CanvasTransform, NodeEntry, Connection as ConnectionType, PinDirection } from '../types';
import { Node } from './Node';
import { Connection } from './Connection';
import { useContext } from 'react';
import { EditorContext } from '../context/EditorContext';
import { computeConnectionRenderData, computeTempConnectionRenderData } from '../utils';

/**
 * @brief BlueprintCanvas Component
 * @description Renders a canvas for blueprint visualization and interaction.
 */
export const BlueprintCanvas: React.FC = () => {
  const { nodeRegistry, blueprints, currentBlueprintIndex, addBlueprint, setCurrentBlueprintIndex, setBlueprints } =
    useContext(EditorContext);

  const [canvasTransform, setCanvasTransform] = useState<CanvasTransform>({
    translateX: 0,
    translateY: 0,
    scale: 1
  });

  const canvasRef = useRef<HTMLDivElement>(null);
  const isDraggingRef = useRef(false);
  const isDraggingNodeRef = useRef(false);
  const hoveredPinRef = useRef<{ pinId: string; direction: PinDirection; pinType: string } | null>(null);
  const [connectingFrom, setConnectingFrom] = useState<{
    pinId: string;
    direction: PinDirection;
    pinType: string;
    element: HTMLElement;
  } | null>(null);
  const [mousePosition, setMousePosition] = useState<{ x: number; y: number } | null>(null);
  const [renderConnections, setRenderConnections] = useState<
    Array<{
      connection: ConnectionType;
      path: string;
      color: string;
      x: number;
      y: number;
      width: number;
      height: number;
    }>
  >([]);
  const [forceUpdate, setForceUpdate] = useState(0);
  const [selectedConnectionId, setSelectedConnectionId] = useState<string | null>(null);
  const [selectedNodeId, setSelectedNodeId] = useState<string | null>(null);
  const [isAltPressed, setIsAltPressed] = useState(false);
  const [isCtrlPressed, setIsCtrlPressed] = useState(false);

  const handleWheel = (event: WheelEvent) => {
    event.preventDefault();

    if (!canvasRef.current) return;

    // Get the bounding rect of the canvas
    const rect = canvasRef.current.getBoundingClientRect();

    // Get mouse position relative to the canvas
    const mouseX = event.clientX - rect.left;
    const mouseY = event.clientY - rect.top;

    // Calculate the new scale
    const delta = event.deltaY * -0.001;
    const newScale = Math.min(Math.max(0.1, canvasTransform.scale + delta), 4);

    // Calculate the world position of the mouse before scaling
    const worldX = (mouseX - canvasTransform.translateX) / canvasTransform.scale;
    const worldY = (mouseY - canvasTransform.translateY) / canvasTransform.scale;

    // Calculate new translation to keep the mouse position fixed
    const newTranslateX = mouseX - worldX * newScale;
    const newTranslateY = mouseY - worldY * newScale;

    setCanvasTransform({
      translateX: newTranslateX,
      translateY: newTranslateY,
      scale: newScale
    });
  };

  const handleMouseDown = (event: MouseEvent) => {
    // Handle left click on canvas background to deselect node
    if (event.button === 0 && !event.shiftKey) {
      const target = event.target as HTMLElement;
      // Check if click is on canvas background (not on node, pin, or connection)
      if (target.classList.contains('canvas') || target.classList.contains('layer')) {
        setSelectedNodeId(null);
      }
    }

    // Only handle middle mouse button or space + left click for panning
    if (event.button !== 1 && !(event.button === 0 && event.shiftKey)) {
      return;
    }

    event.preventDefault();
    isDraggingRef.current = true;

    const startX = event.clientX;
    const startY = event.clientY;
    const startTranslateX = canvasTransform.translateX;
    const startTranslateY = canvasTransform.translateY;

    const onMouseMove = (moveEvent: MouseEvent) => {
      if (!isDraggingRef.current) return;

      const deltaX = moveEvent.clientX - startX;
      const deltaY = moveEvent.clientY - startY;
      setCanvasTransform({
        ...canvasTransform,
        translateX: startTranslateX + deltaX,
        translateY: startTranslateY + deltaY
      });
    };

    const onMouseUp = () => {
      isDraggingRef.current = false;
      window.removeEventListener('mousemove', onMouseMove);
      window.removeEventListener('mouseup', onMouseUp);
    };

    window.addEventListener('mousemove', onMouseMove);
    window.addEventListener('mouseup', onMouseUp);
  };

  // Handle connection start from a pin
  const handleConnectionStart = (pinId: string, direction: PinDirection, pinType: string) => {
    if (!canvasRef.current) return;

    const pinElement = canvasRef.current.querySelector(`.pin[data-id="${pinId}"] .clink`) as HTMLElement;

    if (pinElement) {
      setConnectingFrom({ pinId, direction, pinType, element: pinElement });
    }
  };

  // Handle pin hover
  const handlePinHover = (pinId: string, direction: PinDirection, pinType: string, isHovering: boolean) => {
    if (isHovering) {
      hoveredPinRef.current = { pinId, direction, pinType };
    } else {
      hoveredPinRef.current = null;
    }
  };

  // Handle connection end on a pin
  const handleConnectionEnd = (pinId: string, direction: PinDirection, pinType: string) => {
    if (!connectingFrom || currentBlueprintIndex < 0) return;

    // Reset mouse position
    setMousePosition(null);

    // Prevent connecting same direction pins or same pin
    if (connectingFrom.direction === direction || connectingFrom.pinId === pinId) {
      setConnectingFrom(null);
      return;
    }

    // Prevent connecting different pin types (except exec which can connect to anything)
    if (connectingFrom.pinType !== pinType && connectingFrom.pinType !== 'exec' && pinType !== 'exec') {
      console.warn(`Cannot connect ${connectingFrom.pinType} to ${pinType}. Pin types must match.`);
      setConnectingFrom(null);
      return;
    }

    // Find the nodes that contain these pins
    const currentBlueprint = blueprints[currentBlueprintIndex];
    let sourceNodeId = '';
    let targetNodeId = '';
    let sourcePinId = '';
    let targetPinId = '';

    // Output pins should be source, input pins should be target
    if (direction === 'input') {
      sourcePinId = connectingFrom.pinId;
      targetPinId = pinId;
    } else {
      sourcePinId = pinId;
      targetPinId = connectingFrom.pinId;
    }

    // Find node IDs
    for (const node of currentBlueprint.nodes) {
      const allPins = [...(node.data.inputs || []), ...(node.data.outputs || [])];
      if (allPins.some((p) => p.id === sourcePinId)) {
        sourceNodeId = node.data.id;
      }
      if (allPins.some((p) => p.id === targetPinId)) {
        targetNodeId = node.data.id;
      }
    }

    if (!sourceNodeId || !targetNodeId) {
      setConnectingFrom(null);
      return;
    }

    // Remove any existing connection to the target pin (input can only have one connection)
    const connectionsWithoutTarget = currentBlueprint.connections.filter((conn) => conn.targetPinId !== targetPinId);

    // Create new connection
    const newConnection: ConnectionType = {
      id: `${sourcePinId},${targetPinId}`,
      sourcePinId,
      targetPinId,
      sourceNodeId,
      targetNodeId
    };

    // Update blueprint with new connection
    const updatedBlueprints = [...blueprints];
    updatedBlueprints[currentBlueprintIndex] = {
      ...currentBlueprint,
      connections: [...connectionsWithoutTarget, newConnection]
    };

    setBlueprints(updatedBlueprints);
    setConnectingFrom(null);
  };

  // Handle connection deletion
  const handleDeleteConnection = (connectionId: string) => {
    if (currentBlueprintIndex < 0) return;

    const currentBlueprint = blueprints[currentBlueprintIndex];
    const updatedConnections = currentBlueprint.connections.filter((conn) => conn.id !== connectionId);

    const updatedBlueprints = [...blueprints];
    updatedBlueprints[currentBlueprintIndex] = {
      ...currentBlueprint,
      connections: updatedConnections
    };

    setBlueprints(updatedBlueprints);
    setSelectedConnectionId(null);
  };

  // Handle connection disruption (CTRL+drag on connected pin)
  const handleDisruptConnection = (pinId: string, _direction: PinDirection, pinType: string) => {
    if (currentBlueprintIndex < 0) return;

    const currentBlueprint = blueprints[currentBlueprintIndex];

    // Find and remove the connection involving this pin
    const connectionToRemove = currentBlueprint.connections.find(
      (conn) => conn.sourcePinId === pinId || conn.targetPinId === pinId
    );

    if (connectionToRemove) {
      const updatedConnections = currentBlueprint.connections.filter((conn) => conn.id !== connectionToRemove.id);

      const updatedBlueprints = [...blueprints];
      updatedBlueprints[currentBlueprintIndex] = {
        ...currentBlueprint,
        connections: updatedConnections
      };

      setBlueprints(updatedBlueprints);

      // Start a new connection from the OPPOSITE end of the removed connection
      // If user clicked on an input (target), start from the output (source)
      // If user clicked on an output (source), start from the input (target)
      let newConnectionPinId: string;
      let newConnectionDirection: PinDirection;

      if (connectionToRemove.targetPinId === pinId) {
        // User clicked on input/target, so start from output/source
        newConnectionPinId = connectionToRemove.sourcePinId;
        newConnectionDirection = 'output';
      } else {
        // User clicked on output/source, so start from input/target
        newConnectionPinId = connectionToRemove.targetPinId;
        newConnectionDirection = 'input';
      }

      // Find the pin type of the opposite pin
      let newPinType = pinType;
      for (const node of currentBlueprint.nodes) {
        const allPins = [...(node.data.inputs || []), ...(node.data.outputs || [])];
        const pin = allPins.find((p) => p.id === newConnectionPinId);
        if (pin) {
          newPinType = pin.type;
          break;
        }
      }

      const pinElement = document.querySelector(`.pin[data-id="${newConnectionPinId}"] .clink`) as HTMLElement;
      if (pinElement) {
        setConnectingFrom({
          pinId: newConnectionPinId,
          direction: newConnectionDirection,
          pinType: newPinType,
          element: pinElement
        });
      }
    }
  };

  // Handle node drag start
  const handleNodeDragStart = (nodeId: string) => {
    isDraggingNodeRef.current = true;
    setSelectedNodeId(nodeId);
  };

  // Handle node drag end
  const handleNodeDragEnd = () => {
    isDraggingNodeRef.current = false;
    // Force recompute connections after drag ends
    setTimeout(() => setForceUpdate((prev) => prev + 1), 0);
  };

  // Continuously update connections while dragging
  useEffect(() => {
    if (!isDraggingNodeRef.current) return;

    let animationFrameId: number;
    const updateConnections = () => {
      setForceUpdate((prev) => prev + 1);
      animationFrameId = requestAnimationFrame(updateConnections);
    };

    animationFrameId = requestAnimationFrame(updateConnections);

    return () => {
      if (animationFrameId) {
        cancelAnimationFrame(animationFrameId);
      }
    };
  }, [isDraggingNodeRef.current]);

  // Handle node position change
  const handleNodePositionChange = (nodeId: string, position: { x: number; y: number }) => {
    if (currentBlueprintIndex < 0) return;

    const currentBlueprint = blueprints[currentBlueprintIndex];
    const updatedNodes = currentBlueprint.nodes.map((node) => (node.data.id === nodeId ? { ...node, position } : node));

    const updatedBlueprints = [...blueprints];
    updatedBlueprints[currentBlueprintIndex] = {
      ...currentBlueprint,
      nodes: updatedNodes
    };

    setBlueprints(updatedBlueprints);
  };

  // Handle node click (for selection)
  const handleNodeClick = (nodeId: string) => {
    setSelectedNodeId(nodeId);
  };

  // Recompute connections whenever nodes move or connections change
  useEffect(() => {
    if (currentBlueprintIndex < 0 || !canvasRef.current) return;

    const recomputeConnections = () => {
      if (!canvasRef.current) return;

      const currentBlueprint = blueprints[currentBlueprintIndex];
      const canvasRect = canvasRef.current.getBoundingClientRect();
      const newRenderConnections: typeof renderConnections = [];

      for (const connection of currentBlueprint.connections) {
        const sourcePin = canvasRef.current.querySelector(
          `.pin[data-id="${connection.sourcePinId}"] .clink`
        ) as HTMLElement;
        const targetPin = canvasRef.current.querySelector(
          `.pin[data-id="${connection.targetPinId}"] .clink`
        ) as HTMLElement;

        if (sourcePin && targetPin) {
          const renderData = computeConnectionRenderData(sourcePin, targetPin, canvasRect, canvasTransform.scale);
          if (renderData) {
            newRenderConnections.push({
              connection,
              ...renderData
            });
          }
        }
      }

      setRenderConnections(newRenderConnections);
    };

    // Use requestAnimationFrame to ensure DOM is updated
    requestAnimationFrame(recomputeConnections);
  }, [blueprints, currentBlueprintIndex, canvasTransform, forceUpdate]);

  // Track mouse movement when connecting
  useEffect(() => {
    if (!connectingFrom) return;

    const handleMouseMove = (event: MouseEvent) => {
      setMousePosition({ x: event.clientX, y: event.clientY });
    };

    const handleMouseUp = () => {
      // If we're hovering over a valid pin, try to connect
      if (hoveredPinRef.current) {
        handleConnectionEnd(
          hoveredPinRef.current.pinId,
          hoveredPinRef.current.direction,
          hoveredPinRef.current.pinType
        );
      }

      setConnectingFrom(null);
      setMousePosition(null);
      hoveredPinRef.current = null;
    };

    window.addEventListener('mousemove', handleMouseMove);
    window.addEventListener('mouseup', handleMouseUp);

    return () => {
      window.removeEventListener('mousemove', handleMouseMove);
      window.removeEventListener('mouseup', handleMouseUp);
    };
  }, [connectingFrom]);

  // Attach event listeners with proper cleanup
  useEffect(() => {
    const canvasElement = canvasRef.current;
    if (!canvasElement) return;

    canvasElement.addEventListener('wheel', handleWheel, { passive: false });
    canvasElement.addEventListener('mousedown', handleMouseDown);

    return () => {
      canvasElement.removeEventListener('wheel', handleWheel);
      canvasElement.removeEventListener('mousedown', handleMouseDown);
    };
  }, [canvasTransform]);

  // Keyboard event listeners for ALT and CTRL keys
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.key === 'Alt') {
        setIsAltPressed(true);
      }
      if (event.key === 'Control') {
        setIsCtrlPressed(true);
      }
    };

    const handleKeyUp = (event: KeyboardEvent) => {
      if (event.key === 'Alt') {
        setIsAltPressed(false);
        setSelectedConnectionId(null);
      }
      if (event.key === 'Control') {
        setIsCtrlPressed(false);
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    window.addEventListener('keyup', handleKeyUp);

    return () => {
      window.removeEventListener('keydown', handleKeyDown);
      window.removeEventListener('keyup', handleKeyUp);
    };
  }, []);

  // Initialize blueprint on mount if needed
  useEffect(() => {
    if (currentBlueprintIndex === -1 && blueprints.length === 0) {
      const newBlueprint: BlueprintData = {
        className: 'EmptyBlueprint',
        nodes: [],
        connections: [],
        type: 'AActor',
        properties: [],
        functions: []
      };

      const templates = nodeRegistry.getAllTemplates();
      newBlueprint.nodes = templates
        .map((template) => nodeRegistry.generateNodeDataFromTemplate(template))
        .filter((x) => x)
        .map((data, index) => {
          return {
            data: data,
            position: { x: 150 * (index % 10), y: 100 * Math.floor(index / 10) }
          } as NodeEntry;
        });

      addBlueprint(newBlueprint);
      setCurrentBlueprintIndex(0);
    }
  }, [currentBlueprintIndex, blueprints.length, nodeRegistry, addBlueprint, setCurrentBlueprintIndex]);

  const currentBlueprint =
    currentBlueprintIndex >= 0 && currentBlueprintIndex < blueprints.length ? blueprints[currentBlueprintIndex] : null;

  // Mark pins as filled based on connections
  const filledPins = new Set<string>();
  if (currentBlueprint) {
    for (const conn of currentBlueprint.connections) {
      filledPins.add(conn.sourcePinId);
      filledPins.add(conn.targetPinId);
    }
  }

  // Update node data with filled pins
  const nodesWithFilledPins = currentBlueprint
    ? currentBlueprint.nodes.map((entry) => {
        const updatedData = { ...entry.data };
        if (updatedData.inputs) {
          updatedData.inputs = updatedData.inputs.map((pin) => ({
            ...pin,
            filled: filledPins.has(pin.id)
          }));
        }
        if (updatedData.outputs) {
          updatedData.outputs = updatedData.outputs.map((pin) => ({
            ...pin,
            filled: filledPins.has(pin.id)
          }));
        }
        return { ...entry, data: updatedData };
      })
    : [];

  const nodes: ReactElement<Node>[] = nodesWithFilledPins.map((entry, index) => (
    <Node
      key={entry.data.id || index}
      data={entry.data}
      position={entry.position}
      dimension={entry.dimensions}
      selected={selectedNodeId === entry.data.id}
      onPositionChange={handleNodePositionChange}
      onDragStart={handleNodeDragStart}
      onDragEnd={handleNodeDragEnd}
      onConnectionStart={handleConnectionStart}
      onConnectionEnd={handleConnectionEnd}
      onPinHover={handlePinHover}
      onDisruptConnection={handleDisruptConnection}
      onClick={handleNodeClick}
      scale={canvasTransform.scale}
      isCtrlPressed={isCtrlPressed}
    />
  ));

  // Handle connection click
  const handleConnectionClick = (connectionId: string) => {
    if (isAltPressed) {
      // Delete connection when ALT is pressed
      handleDeleteConnection(connectionId);
    } else {
      // Just select the connection
      setSelectedConnectionId(connectionId);
    }
  };

  const connections = renderConnections.map((conn) => (
    <Connection
      key={conn.connection.id}
      connectionId={conn.connection.id}
      path={conn.path}
      color={conn.color}
      x={conn.x}
      y={conn.y}
      width={conn.width}
      height={conn.height}
      isSelected={isAltPressed && selectedConnectionId === conn.connection.id}
      isDimmed={isAltPressed && selectedConnectionId !== null && selectedConnectionId !== conn.connection.id}
      onClick={handleConnectionClick}
    />
  ));

  // Render temporary connection while dragging
  let tempConnection: ReactElement | null = null;
  if (connectingFrom && mousePosition) {
    const tempData = computeTempConnectionRenderData(
      connectingFrom.element,
      mousePosition,
      canvasTransform,
      connectingFrom.direction
    );
    if (tempData) {
      tempConnection = (
        <Connection
          key='temp-connection'
          connectionId='temp'
          path={tempData.path}
          color={tempData.color}
          x={tempData.x}
          y={tempData.y}
          width={tempData.width}
          height={tempData.height}
        />
      );
    }
  }

  return (
    <div className='layer' ref={canvasRef}>
      <div
        className='reference'
        style={{
          transform: `translate(${canvasTransform.translateX}px, ${canvasTransform.translateY}px) scale(${canvasTransform.scale})`
        }}
      ></div>
      <div
        className='canvas'
        style={{
          transform: `translate(${canvasTransform.translateX}px, ${canvasTransform.translateY}px) scale(${canvasTransform.scale})`,
          cursor: isDraggingRef.current ? 'grabbing' : 'default'
        }}
      >
        {connections}
        {tempConnection}
        {nodes}
      </div>
    </div>
  );
};
