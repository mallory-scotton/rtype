/** Dependencies */
import { useState, useEffect, useRef, type ReactElement } from 'react';
import type {
  BlueprintData,
  CanvasTransform,
  NodeEntry,
  Connection as ConnectionType,
  PinDirection
} from '../../types';
import { Node } from './Node';
import { Connection } from './Connection';
import { ContextMenu } from './ContextMenu';
import { useEditor } from '../../context';
import { computeConnectionRenderData, computeTempConnectionRenderData } from '../../utils';

/**
 * @brief BlueprintCanvas Component
 * @description Renders a canvas for blueprint visualization and interaction.
 */
export const BlueprintCanvas: React.FC = () => {
  const { nodeRegistry, blueprints, currentBlueprintIndex, addBlueprint, setCurrentBlueprintIndex, setBlueprints } =
    useEditor();

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
  const [selectedNodeIds, setSelectedNodeIds] = useState<string[]>([]);
  const [isAltPressed, setIsAltPressed] = useState(false);
  const [isCtrlPressed, setIsCtrlPressed] = useState(false);
  const [contextMenu, setContextMenu] = useState<{ x: number; y: number } | null>(null);
  const [multiSelectBox, setMultiSelectBox] = useState<{
    startX: number;
    startY: number;
    currentX: number;
    currentY: number;
  } | null>(null);
  const isDrawingMultiSelect = useRef(false);
  const multiSelectStartNodeIds = useRef<string[]>([]);
  const dragStartPositions = useRef<Map<string, { x: number; y: number }>>(new Map());
  const draggedNodeId = useRef<string | null>(null);

  const handleWheel = (event: WheelEvent) => {
    // Don't prevent default or handle wheel if we're scrolling in the context menu
    if (contextMenu) {
      const target = event.target as HTMLElement;
      // Check if the target is within a context menu or scrollable area
      if (target.closest('[data-context-menu]')) {
        return; // Let the context menu handle its own scrolling
      }
    }

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
    const target = event.target as HTMLElement;
    const isCanvasBackground =
      target.classList.contains('canvas') ||
      target.classList.contains('layer') ||
      target.classList.contains('reference');

    // Close context menu if clicking outside of it
    if (contextMenu && !target.closest('[data-context-menu]')) {
      setContextMenu(null);
    }

    // Handle left click for multi-select or deselection
    if (event.button === 0) {
      if (isCanvasBackground && !event.shiftKey && !event.ctrlKey) {
        // Start multi-select box or deselect all
        if (!canvasRef.current) return;

        const rect = canvasRef.current.getBoundingClientRect();
        const startX = (event.clientX - rect.left - canvasTransform.translateX) / canvasTransform.scale;
        const startY = (event.clientY - rect.top - canvasTransform.translateY) / canvasTransform.scale;

        isDrawingMultiSelect.current = true;
        multiSelectStartNodeIds.current = [...selectedNodeIds];
        setMultiSelectBox({
          startX,
          startY,
          currentX: startX,
          currentY: startY
        });

        // Don't deselect yet - wait to see if it's a drag or just a click
        return;
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

  // Handle pin value change
  const handlePinValueChange = (pinId: string, value: any) => {
    if (currentBlueprintIndex < 0) return;

    const currentBlueprint = blueprints[currentBlueprintIndex];
    const updatedNodes = currentBlueprint.nodes.map((node) => {
      const updatedInputs = node.data.inputs?.map((pin) => {
        if (pin.id === pinId) {
          return { ...pin, value };
        }
        return pin;
      });

      return {
        ...node,
        data: {
          ...node.data,
          inputs: updatedInputs
        }
      };
    });

    const updatedBlueprints = [...blueprints];
    updatedBlueprints[currentBlueprintIndex] = {
      ...currentBlueprint,
      nodes: updatedNodes
    };

    setBlueprints(updatedBlueprints);
  };

  // Handle node drag start
  const handleNodeDragStart = (nodeId: string) => {
    isDraggingNodeRef.current = true;
    draggedNodeId.current = nodeId;

    // Add to selection if not already selected
    if (!selectedNodeIds.includes(nodeId)) {
      setSelectedNodeIds([nodeId]);
      // Store only this node's position
      if (currentBlueprintIndex >= 0) {
        const node = blueprints[currentBlueprintIndex].nodes.find((n) => n.data.id === nodeId);
        if (node) {
          dragStartPositions.current.clear();
          dragStartPositions.current.set(nodeId, { ...node.position });
        }
      }
    } else {
      // Store positions of all selected nodes
      if (currentBlueprintIndex >= 0) {
        dragStartPositions.current.clear();
        for (const id of selectedNodeIds) {
          const node = blueprints[currentBlueprintIndex].nodes.find((n) => n.data.id === id);
          if (node) {
            dragStartPositions.current.set(id, { ...node.position });
          }
        }
      }
    }
  };

  // Handle node drag end
  const handleNodeDragEnd = () => {
    isDraggingNodeRef.current = false;
    draggedNodeId.current = null;
    dragStartPositions.current.clear();
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

    // If this is the dragged node and multiple nodes are selected, move all selected nodes
    if (isDraggingNodeRef.current && draggedNodeId.current === nodeId && selectedNodeIds.length > 1) {
      const startPos = dragStartPositions.current.get(nodeId);
      if (!startPos) {
        // Fallback to single node update
        const updatedNodes = currentBlueprint.nodes.map((node) =>
          node.data.id === nodeId ? { ...node, position } : node
        );

        const updatedBlueprints = [...blueprints];
        updatedBlueprints[currentBlueprintIndex] = {
          ...currentBlueprint,
          nodes: updatedNodes
        };
        setBlueprints(updatedBlueprints);
        return;
      }

      // Calculate delta from the dragged node's start position
      const deltaX = position.x - startPos.x;
      const deltaY = position.y - startPos.y;

      // Update all selected nodes
      const updatedNodes = currentBlueprint.nodes.map((node) => {
        if (selectedNodeIds.includes(node.data.id)) {
          const nodeStartPos = dragStartPositions.current.get(node.data.id);
          if (nodeStartPos) {
            return {
              ...node,
              position: {
                x: nodeStartPos.x + deltaX,
                y: nodeStartPos.y + deltaY
              }
            };
          }
        }
        return node;
      });

      const updatedBlueprints = [...blueprints];
      updatedBlueprints[currentBlueprintIndex] = {
        ...currentBlueprint,
        nodes: updatedNodes
      };
      setBlueprints(updatedBlueprints);
    } else {
      // Single node update
      const updatedNodes = currentBlueprint.nodes.map((node) =>
        node.data.id === nodeId ? { ...node, position } : node
      );

      const updatedBlueprints = [...blueprints];
      updatedBlueprints[currentBlueprintIndex] = {
        ...currentBlueprint,
        nodes: updatedNodes
      };
      setBlueprints(updatedBlueprints);
    }
  };

  // Handle node click (for selection)
  const handleNodeClick = (nodeId: string, event?: React.MouseEvent) => {
    if (event?.ctrlKey || event?.metaKey) {
      // Toggle selection with Ctrl
      if (selectedNodeIds.includes(nodeId)) {
        setSelectedNodeIds(selectedNodeIds.filter((id) => id !== nodeId));
      } else {
        setSelectedNodeIds([...selectedNodeIds, nodeId]);
      }
    } else if (event?.shiftKey) {
      // Add to selection with Shift
      if (!selectedNodeIds.includes(nodeId)) {
        setSelectedNodeIds([...selectedNodeIds, nodeId]);
      }
    } else {
      // If clicking on an already selected node, don't change selection (allows dragging multiple nodes)
      // If clicking on an unselected node, replace selection
      if (!selectedNodeIds.includes(nodeId)) {
        setSelectedNodeIds([nodeId]);
      }
      // Note: If the node is already selected, we keep the current selection
      // This allows users to drag multiple selected nodes together
    }
  };

  // Handle context menu (right click)
  const handleContextMenu = (event: MouseEvent) => {
    const target = event.target as HTMLElement;
    // Only show context menu on canvas or reference background
    if (target.classList.contains('canvas') || target.classList.contains('reference')) {
      event.preventDefault();
      setContextMenu({ x: event.clientX, y: event.clientY });
    }
  };

  // Handle node spawn from context menu
  const handleSpawnNode = (templateId: string) => {
    if (currentBlueprintIndex < 0 || !contextMenu || !canvasRef.current) {
      return;
    }

    const currentBlueprint = blueprints[currentBlueprintIndex];
    const template = nodeRegistry.getTemplate(templateId);

    if (!template) {
      return;
    }

    const nodeData = nodeRegistry.generateNodeDataFromTemplate(template);

    if (!nodeData) {
      return;
    }

    // Convert screen position to canvas position
    const rect = canvasRef.current.getBoundingClientRect();
    const canvasX = (contextMenu.x - rect.left - canvasTransform.translateX) / canvasTransform.scale;
    const canvasY = (contextMenu.y - rect.top - canvasTransform.translateY) / canvasTransform.scale;

    const newNode: NodeEntry = {
      template: templateId,
      data: nodeData,
      position: { x: canvasX, y: canvasY }
    };

    const updatedBlueprints = [...blueprints];
    updatedBlueprints[currentBlueprintIndex] = {
      ...currentBlueprint,
      nodes: [...currentBlueprint.nodes, newNode]
    };

    setBlueprints(updatedBlueprints);
    setContextMenu(null);
  };

  // Handle nodes deletion
  const handleDeleteNodes = (nodeIds: string[]) => {
    if (currentBlueprintIndex < 0 || nodeIds.length === 0) return;

    const currentBlueprint = blueprints[currentBlueprintIndex];
    const nodeIdsSet = new Set(nodeIds);

    // Remove the nodes
    const updatedNodes = currentBlueprint.nodes.filter((node) => !nodeIdsSet.has(node.data.id));

    // Remove all connections involving these nodes
    const updatedConnections = currentBlueprint.connections.filter(
      (conn) => !nodeIdsSet.has(conn.sourceNodeId) && !nodeIdsSet.has(conn.targetNodeId)
    );

    const updatedBlueprints = [...blueprints];
    updatedBlueprints[currentBlueprintIndex] = {
      ...currentBlueprint,
      nodes: updatedNodes,
      connections: updatedConnections
    };

    setBlueprints(updatedBlueprints);
    setSelectedNodeIds(selectedNodeIds.filter((id) => !nodeIdsSet.has(id)));
  };

  // Handle moving multiple nodes at once (for arrow keys)
  const handleMoveNodes = (nodeIds: string[], dx: number, dy: number) => {
    if (currentBlueprintIndex < 0 || nodeIds.length === 0) return;

    const currentBlueprint = blueprints[currentBlueprintIndex];
    const nodeIdsSet = new Set(nodeIds);

    // Update all nodes in a single operation
    const updatedNodes = currentBlueprint.nodes.map((node) => {
      if (nodeIdsSet.has(node.data.id)) {
        return {
          ...node,
          position: {
            x: node.position.x + dx,
            y: node.position.y + dy
          }
        };
      }
      return node;
    });

    const updatedBlueprints = [...blueprints];
    updatedBlueprints[currentBlueprintIndex] = {
      ...currentBlueprint,
      nodes: updatedNodes
    };

    setBlueprints(updatedBlueprints);
  };

  // Recompute connections whenever nodes move or connections change
  useEffect(() => {
    if (currentBlueprintIndex < 0 || !canvasRef.current) return;

    const recomputeConnections = () => {
      if (!canvasRef.current) return;

      const currentBlueprint = blueprints[currentBlueprintIndex];
      const newRenderConnections: typeof renderConnections = [];

      for (const connection of currentBlueprint.connections) {
        const sourcePin = canvasRef.current.querySelector(
          `.pin[data-id="${connection.sourcePinId}"] .clink`
        ) as HTMLElement;
        const targetPin = canvasRef.current.querySelector(
          `.pin[data-id="${connection.targetPinId}"] .clink`
        ) as HTMLElement;

        if (sourcePin && targetPin) {
          const renderData = computeConnectionRenderData(sourcePin, targetPin, canvasTransform.scale);
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

  // Handle multi-select box drawing
  useEffect(() => {
    if (!isDrawingMultiSelect.current) return;

    const handleMouseMove = (event: MouseEvent) => {
      if (!isDrawingMultiSelect.current || !canvasRef.current || !multiSelectBox) return;

      const rect = canvasRef.current.getBoundingClientRect();
      const currentX = (event.clientX - rect.left - canvasTransform.translateX) / canvasTransform.scale;
      const currentY = (event.clientY - rect.top - canvasTransform.translateY) / canvasTransform.scale;

      setMultiSelectBox({
        ...multiSelectBox,
        currentX,
        currentY
      });

      // Calculate selection rectangle
      const left = Math.min(multiSelectBox.startX, currentX);
      const top = Math.min(multiSelectBox.startY, currentY);
      const right = Math.max(multiSelectBox.startX, currentX);
      const bottom = Math.max(multiSelectBox.startY, currentY);

      // Check which nodes intersect with the selection box
      if (currentBlueprintIndex >= 0) {
        const currentBlueprint = blueprints[currentBlueprintIndex];
        const newSelectedIds: string[] = [];

        for (const node of currentBlueprint.nodes) {
          const nodeElement = canvasRef.current.querySelector(`.node[data-id="${node.data.id}"]`) as HTMLElement;
          if (!nodeElement) continue;

          const nodeRect = nodeElement.getBoundingClientRect();
          const nodeLeft = (nodeRect.left - rect.left - canvasTransform.translateX) / canvasTransform.scale;
          const nodeTop = (nodeRect.top - rect.top - canvasTransform.translateY) / canvasTransform.scale;
          const nodeRight = nodeLeft + nodeRect.width / canvasTransform.scale;
          const nodeBottom = nodeTop + nodeRect.height / canvasTransform.scale;

          // Check if node overlaps with selection box
          if (left < nodeRight && right > nodeLeft && top < nodeBottom && bottom > nodeTop) {
            // If Ctrl is pressed and node was already selected, don't add it (inverse selection)
            const wasAlreadySelected = multiSelectStartNodeIds.current.includes(node.data.id);
            if (event.ctrlKey || event.metaKey) {
              if (!wasAlreadySelected) {
                newSelectedIds.push(node.data.id);
              }
            } else if (event.shiftKey) {
              // Shift key: only add to selection if it wasn't already selected
              if (wasAlreadySelected || !multiSelectStartNodeIds.current.includes(node.data.id)) {
                newSelectedIds.push(node.data.id);
              }
            } else {
              newSelectedIds.push(node.data.id);
            }
          } else {
            // Node outside selection box
            if (event.shiftKey && multiSelectStartNodeIds.current.includes(node.data.id)) {
              // Keep previously selected nodes with Shift
              newSelectedIds.push(node.data.id);
            } else if ((event.ctrlKey || event.metaKey) && multiSelectStartNodeIds.current.includes(node.data.id)) {
              // Keep previously selected nodes with Ctrl
              newSelectedIds.push(node.data.id);
            }
          }
        }

        setSelectedNodeIds(newSelectedIds);
      }
    };

    const handleMouseUp = () => {
      if (!isDrawingMultiSelect.current) return;

      isDrawingMultiSelect.current = false;

      // If no drag happened (click), deselect all
      if (
        multiSelectBox &&
        Math.abs(multiSelectBox.currentX - multiSelectBox.startX) < 2 &&
        Math.abs(multiSelectBox.currentY - multiSelectBox.startY) < 2
      ) {
        setSelectedNodeIds([]);
      }

      setMultiSelectBox(null);
      multiSelectStartNodeIds.current = [];
    };

    window.addEventListener('mousemove', handleMouseMove);
    window.addEventListener('mouseup', handleMouseUp);

    return () => {
      window.removeEventListener('mousemove', handleMouseMove);
      window.removeEventListener('mouseup', handleMouseUp);
    };
  }, [multiSelectBox, canvasTransform, currentBlueprintIndex, blueprints]);

  // Attach event listeners with proper cleanup
  useEffect(() => {
    const canvasElement = canvasRef.current;
    if (!canvasElement) return;

    canvasElement.addEventListener('wheel', handleWheel, { passive: false });
    canvasElement.addEventListener('mousedown', handleMouseDown);
    canvasElement.addEventListener('contextmenu', handleContextMenu);

    return () => {
      canvasElement.removeEventListener('wheel', handleWheel);
      canvasElement.removeEventListener('mousedown', handleMouseDown);
      canvasElement.removeEventListener('contextmenu', handleContextMenu);
    };
  }, [canvasTransform, contextMenu]);

  // Keyboard event listeners for ALT, CTRL, Delete, ESC, and Arrow keys
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.key === 'Alt') {
        setIsAltPressed(true);
      }
      if (event.key === 'Control') {
        setIsCtrlPressed(true);
      }

      // Handle Escape key to close context menu
      if (event.key === 'Escape' && contextMenu) {
        event.preventDefault();
        setContextMenu(null);
      }

      // Handle Delete key - delete all selected nodes
      if (event.key === 'Delete' && selectedNodeIds.length > 0) {
        event.preventDefault();
        handleDeleteNodes(selectedNodeIds);
      }

      // Handle arrow keys for moving selected nodes
      if (selectedNodeIds.length > 0 && currentBlueprintIndex >= 0) {
        let moved = false;
        const moveAmount = event.shiftKey ? 10 : 1; // Hold shift for larger movements
        let dx = 0;
        let dy = 0;

        switch (event.key) {
          case 'ArrowUp':
            event.preventDefault();
            dy = -moveAmount;
            moved = true;
            break;
          case 'ArrowDown':
            event.preventDefault();
            dy = moveAmount;
            moved = true;
            break;
          case 'ArrowLeft':
            event.preventDefault();
            dx = -moveAmount;
            moved = true;
            break;
          case 'ArrowRight':
            event.preventDefault();
            dx = moveAmount;
            moved = true;
            break;
        }

        if (moved) {
          // Move all selected nodes at once
          handleMoveNodes(selectedNodeIds, dx, dy);
        }
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
  }, [selectedNodeIds, currentBlueprintIndex, blueprints, contextMenu]);

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
      selected={selectedNodeIds.includes(entry.data.id)}
      onPositionChange={handleNodePositionChange}
      onDragStart={handleNodeDragStart}
      onDragEnd={handleNodeDragEnd}
      onConnectionStart={handleConnectionStart}
      onConnectionEnd={handleConnectionEnd}
      onPinHover={handlePinHover}
      onDisruptConnection={handleDisruptConnection}
      onPinValueChange={handlePinValueChange}
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

        {/* Multi-select box */}
        {multiSelectBox && (
          <div
            className='multi-select'
            style={{
              position: 'absolute',
              left: `${Math.min(multiSelectBox.startX, multiSelectBox.currentX)}px`,
              top: `${Math.min(multiSelectBox.startY, multiSelectBox.currentY)}px`,
              width: `${Math.abs(multiSelectBox.currentX - multiSelectBox.startX)}px`,
              height: `${Math.abs(multiSelectBox.currentY - multiSelectBox.startY)}px`,
              border: '2px dashed rgba(255, 255, 255, 0.5)',
              background: 'rgba(255, 255, 255, 0.1)',
              pointerEvents: 'none',
              zIndex: 1000
            }}
          />
        )}
      </div>

      {/* Context Menu */}
      {contextMenu && (
        <ContextMenu
          position={contextMenu}
          nodeRegistry={nodeRegistry}
          onSpawnNode={handleSpawnNode}
          onClose={() => setContextMenu(null)}
        />
      )}
    </div>
  );
};
