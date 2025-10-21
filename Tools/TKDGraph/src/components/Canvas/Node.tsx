/** Dependencies */
import type { NodeEntry } from '../../types';
import { Pin } from './Pin';
import { useState, useRef, useEffect } from 'react';
import type { MouseEvent } from 'react';
import './Node.css';
import { useEditor } from '../../context';
import { snapToGrid } from '../../utils';

/**
 * @brief Interface for Node component props
 * @description This interface defines the structure of the props that the Node component expects to receive.
 */
interface NodeProps {
  entry: NodeEntry;
}

/**
 * @brief Node component
 * @description This component represents a node in a graph, displaying its data.
 */
export const Node: React.FC<NodeProps> = ({ entry }) => {
  // Use Editor Context
  const {
    selectedNodeIds,
    setSelectedNodeIds,
    blueprints,
    currentBlueprintIndex,
    setBlueprints,
    canvasTransform,
    triggerConnectionUpdate
  } = useEditor();

  // Define CSS classes
  const classes: string[] = ['node'];

  // State for dragging
  const [isDragging, setIsDragging] = useState(false);
  const dragStartPos = useRef({ x: 0, y: 0 });
  const nodeStartPos = useRef({ x: 0, y: 0 });
  const initialNodePositions = useRef<Map<string, { x: number; y: number }>>(new Map());
  const [currentPosition, setCurrentPosition] = useState(entry.position);

  // Update position when entry position changes
  useEffect(() => {
    setCurrentPosition(entry.position);
  }, [entry.position]);

  // Add document-level mouse event listeners for smooth dragging
  useEffect(() => {
    if (!isDragging) return;

    const handleDocumentMouseMove = (event: globalThis.MouseEvent) => {
      handleMouseMove(event);
    };

    const handleDocumentMouseUp = (event: globalThis.MouseEvent) => {
      handleMouseUp(event);
    };

    document.addEventListener('mousemove', handleDocumentMouseMove);
    document.addEventListener('mouseup', handleDocumentMouseUp);

    return () => {
      document.removeEventListener('mousemove', handleDocumentMouseMove);
      document.removeEventListener('mouseup', handleDocumentMouseUp);
    };
  }, [isDragging, blueprints, currentBlueprintIndex, selectedNodeIds, canvasTransform.scale]);

  // Destructure entry
  const { data, dimensions } = entry;

  // Determine node type based on pins
  let allPins = [...(data.inputs || []), ...(data.outputs || [])];
  let nodeType = allPins.find((pin) => pin.type !== 'exec')?.type || '';

  // Apply classes and styles based on node type
  if (data.type === 'getter') {
    classes.push(`nvariableget connector ${nodeType}`);
    data.roundedBg = data.roundedBg ?? true;
  } else if (data.type === 'setter') {
    classes.push(`nvariableset connector ${nodeType}`);
    data.roundedBg = data.roundedBg ?? 'SET';
    data.roundedBgColor = data.roundedBgColor ?? true;
  } else if (data.type === 'cast') {
    classes.push('nconv');
    data.roundedBg = data.roundedBg ?? true;
  } else if (data.type === 'operator') {
    classes.push('nkismetmath');
    data.roundedBg = data.roundedBg ?? true;
  } else if (data.type === 'comment') {
    classes.push('ncomment');
  } else if (data.type === 'knot') {
    classes.push('knot');
  }

  // Highlight if selected
  if (selectedNodeIds.some((id) => id === entry.data.id)) {
    classes.push('selected');
  }

  /**
   * @brief Handle mouse move event for dragging
   */
  const handleMouseMove = (event: globalThis.MouseEvent) => {
    if (!isDragging) return;

    // Get the current blueprint
    const currentBlueprint = blueprints[currentBlueprintIndex];
    if (!currentBlueprint) return;

    const deltaX = (event.clientX - dragStartPos.current.x) / canvasTransform.scale;
    const deltaY = (event.clientY - dragStartPos.current.y) / canvasTransform.scale;

    const newX = nodeStartPos.current.x + deltaX;
    const newY = nodeStartPos.current.y + deltaY;

    setCurrentPosition({ x: newX, y: newY });

    // Update position of all selected nodes using their initial positions
    const updatedNodes = currentBlueprint.nodes.map((node) => {
      if (selectedNodeIds.includes(node.data.id)) {
        const initialPos = initialNodePositions.current.get(node.data.id);
        if (initialPos) {
          return {
            ...node,
            position: {
              x: initialPos.x + deltaX,
              y: initialPos.y + deltaY
            }
          };
        }
      }
      return node;
    });

    // Update blueprint nodes
    const updatedBlueprints = [...blueprints];
    updatedBlueprints[currentBlueprintIndex] = {
      ...currentBlueprint,
      nodes: updatedNodes
    };
    setBlueprints(updatedBlueprints);

    // Trigger connection updates
    triggerConnectionUpdate();
  };

  /**
   * @brief Handle mouse up event to stop dragging
   */
  const handleMouseUp = (event: globalThis.MouseEvent) => {
    if (!isDragging) return;

    setIsDragging(false);

    // Calculate final position based on mouse position at mouseup (not currentPosition)
    const deltaX = (event.clientX - dragStartPos.current.x) / canvasTransform.scale;
    const deltaY = (event.clientY - dragStartPos.current.y) / canvasTransform.scale;
    const finalX = nodeStartPos.current.x + deltaX;
    const finalY = nodeStartPos.current.y + deltaY;

    // Snap to grid on mouse up
    const snappedX = snapToGrid(finalX, 16);
    const snappedY = snapToGrid(finalY, 16);

    // Set snapped position
    setCurrentPosition({ x: snappedX, y: snappedY });

    // Update all selected nodes with snapped positions
    const currentBlueprint = blueprints[currentBlueprintIndex];
    if (currentBlueprint) {
      const updatedNodes = currentBlueprint.nodes.map((node) => {
        if (selectedNodeIds.includes(node.data.id)) {
          const initialPos = initialNodePositions.current.get(node.data.id);
          if (initialPos) {
            return {
              ...node,
              position: {
                x: snapToGrid(initialPos.x + deltaX, 16),
                y: snapToGrid(initialPos.y + deltaY, 16)
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

      // Trigger connection updates after snapping
      // Small delay to ensure React has re-rendered nodes with snapped positions
      setTimeout(() => {
        triggerConnectionUpdate();
      }, 0);
    }

    // Clear initial positions
    initialNodePositions.current.clear();
  };

  const handleMouseDown = (event: MouseEvent) => {
    const target = event.target as HTMLElement;
    // Only handle left mouse button and not on pins
    if (event.button !== 0 || target.closest('.pin')) {
      return;
    }

    // Handle selection
    if (event?.ctrlKey || event?.metaKey) {
      // Toggle selection with Ctrl
      if (selectedNodeIds.includes(data.id)) {
        setSelectedNodeIds(selectedNodeIds.filter((id) => id !== data.id));
      } else {
        setSelectedNodeIds([...selectedNodeIds, data.id]);
      }
    } else if (event?.shiftKey) {
      // Add to selection with Shift
      if (!selectedNodeIds.includes(data.id)) {
        setSelectedNodeIds([...selectedNodeIds, data.id]);
      }
    } else {
      // If clicking on an already selected node, don't change selection (allows dragging multiple nodes)
      // If clicking on an unselected node, replace selection
      if (!selectedNodeIds.includes(data.id)) {
        setSelectedNodeIds([data.id]);
      }
      // Note: If the node is already selected, we keep the current selection
      // This allows users to drag multiple selected nodes together
    }

    event.stopPropagation();
    setIsDragging(true);

    dragStartPos.current = { x: event.clientX, y: event.clientY };
    nodeStartPos.current = { ...currentPosition };

    // Store initial positions of all selected nodes
    const currentBlueprint = blueprints[currentBlueprintIndex];
    if (currentBlueprint) {
      initialNodePositions.current.clear();
      currentBlueprint.nodes.forEach((node) => {
        if (selectedNodeIds.includes(node.data.id) || node.data.id === data.id) {
          initialNodePositions.current.set(node.data.id, { ...node.position });
        }
      });
    }
  };

  return (
    <div
      data-id={data.id}
      style={{
        position: 'absolute',
        transform: `translate(${currentPosition.x}px, ${currentPosition.y}px)`,
        width: dimensions?.width,
        height: dimensions?.height,
        backgroundColor: data.backgroundColor,
        cursor: isDragging ? 'grabbing' : 'grab'
      }}
      className={classes.join(' ')}
      onMouseDown={handleMouseDown}
    >
      {data.header && data.type !== 'comment' && (
        <div className={`header node-color ${data.header.type} gradient`}>
          {data.header.icon && <div className={`icon ${data.header.icon}`}></div>}
          {data.header.label && (
            <span className={`${data.header.icon ? 'has-icon ' : ''}name`}>{data.header.label}</span>
          )}
        </div>
      )}
      {data.type === 'comment' && data.header && (
        <div className='header' style={{ backgroundColor: data.header.backgroundColor }}>
          {data.header.label}
        </div>
      )}
      <div className='body'>
        {data.label && data.type === 'operator' && (
          <div className='center-text'>
            <span>{data.label}</span>
          </div>
        )}
        {data.roundedBgColor && <div className={`round-bg-color connector ${nodeType}`}></div>}
        {data.roundedBg && <div className='round-bg'>{typeof data.roundedBg === 'string' ? data.roundedBg : ''}</div>}
        <div className='left-col'>
          {data.inputs &&
            data.inputs.map((input) => <Pin key={input.id} data={input} direction='input' nodeId={data.id} />)}
        </div>
        <div className='right-col'>
          {data.outputs &&
            data.outputs.map((output) => <Pin key={output.id} data={output} direction='output' nodeId={data.id} />)}
        </div>
      </div>
    </div>
  );
};
