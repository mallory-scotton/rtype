/** Dependencies */
import React, { useEffect, useState } from 'react';
import './Canvas.css';
import { useEditor } from '../../context';
import { Node } from './Node';
import { Connection } from './Connection';
import { TempConnection } from './TempConnection';
import { snapToGrid } from '../../utils';
import { ContextMenu } from './ContextMenu';

/**
 * @brief Props for Canvas component
 * @description Defines the properties required to render the Canvas component.
 */
export interface CanvasProps {
  children?: React.ReactNode | React.ReactNode[];
}

/**
 * @brief Canvas component
 * @description Renders a canvas area with specified transformations (translation and scaling).
 */
export const Canvas: React.FC<CanvasProps> = ({ children }) => {
  // Use Editor Context
  const {
    canvasTransform,
    blueprints,
    currentBlueprintIndex,
    removeSelectedNodesFromCurrentBlueprint,
    setSelectedNodeIds,
    nodeRegistry,
    addNodeToBlueprint,
    canvasRef
  } = useEditor();

  // Shortcut action
  const [isBPressed, setIsBPressed] = useState(false);

  // Get the current blueprint
  const currentBlueprint = blueprints[currentBlueprintIndex];

  // If no blueprint is selected, render an empty canvas
  if (!currentBlueprint) {
    return <div className='canvas' />;
  }

  useEffect(() => {
    // This effect could be used for side effects related to canvas or blueprint changes using keyboards
    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.key === 'Delete' || event.key === 'Backspace') {
        removeSelectedNodesFromCurrentBlueprint();
      } else if (event.key === 'Escape') {
        setSelectedNodeIds([]);
      } else if (event.key === 'b' || event.key === 'B') {
        setIsBPressed(true);
      }
    };

    // Check for keyup to reset B key state
    const handleKeyUp = (event: KeyboardEvent) => {
      if (event.key === 'b' || event.key === 'B') {
        setIsBPressed(false);
      }
    };

    // Check for click
    const handleMouseDown = (event: MouseEvent) => {
      const target = event.target as HTMLElement;
      const canCreateBranch =
        isBPressed &&
        currentBlueprint &&
        ['reference', 'layer', 'canvas'].some((cls) => target.classList.contains(cls)) &&
        event.button === 0;

      if (canCreateBranch) {
        // Prevent default behavior
        event.stopPropagation();
        event.preventDefault();
        // Logic for handling 'B' key + click can be added here
        setIsBPressed(false);

        // Find the template for branch node
        const branchTemplate = nodeRegistry.getTemplate('flow_branch');
        if (branchTemplate) {
          const rect = canvasRef.current?.getBoundingClientRect();
          if (!rect) return;

          const x =
            snapToGrid((event.clientX - rect.left - canvasTransform.translateX) / canvasTransform.scale, 16) - 90;
          const y =
            snapToGrid((event.clientY - rect.top - canvasTransform.translateY) / canvasTransform.scale, 16) - 45;

          addNodeToBlueprint({
            position: { x, y },
            template: 'flow_branch',
            data: nodeRegistry.generateNodeDataFromTemplate(branchTemplate)
          });
        }
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    window.addEventListener('keyup', handleKeyUp);
    window.addEventListener('mousedown', handleMouseDown);
    return () => {
      window.removeEventListener('keydown', handleKeyDown);
      window.removeEventListener('keyup', handleKeyUp);
      window.removeEventListener('mousedown', handleMouseDown);
    };
  }, [
    currentBlueprint,
    removeSelectedNodesFromCurrentBlueprint,
    setSelectedNodeIds,
    setIsBPressed,
    isBPressed,
    nodeRegistry,
    addNodeToBlueprint,
    canvasTransform,
    canvasRef,
    snapToGrid
  ]);

  // Mark pins as filled based on connections
  const filledPins = new Set<string>();
  if (currentBlueprint) {
    for (const connection of currentBlueprint.connections) {
      filledPins.add(connection.sourcePinId);
      filledPins.add(connection.targetPinId);
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

  return (
    <div
      className='canvas'
      style={{
        transform: `translate(${canvasTransform.translateX}px, ${canvasTransform.translateY}px) scale(${canvasTransform.scale})`
      }}
    >
      {/** Render Context Menu */}
      <ContextMenu />

      {/* Render connections first (below nodes) */}
      {currentBlueprint.connections.map((connection) => (
        <Connection key={connection.id} connection={connection} />
      ))}

      {/* Render temporary connection while dragging */}
      <TempConnection />

      {/* Render nodes */}
      {...nodesWithFilledPins.map((entry, index) => <Node key={index} entry={entry} />)}

      {children}
    </div>
  );
};
