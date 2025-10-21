/** Dependencies */
import React, { useEffect, useRef } from 'react';
import './MultiSelect.css';
import { useEditor } from '../../context';

/**
 * @brief Props for MutliSelect component
 * @description Defines the properties required to render the MutliSelect component.
 */
export interface MutliSelectProps {}

/**
 * @brief MutliSelect component
 * @description Renders a multi select interface.
 */
export const MutliSelect: React.FC<MutliSelectProps> = () => {
  const {
    canvasRef,
    canvasTransform,
    setMultiSelectBox,
    multiSelectBox,
    blueprints,
    currentBlueprintIndex,
    setSelectedNodeIds,
    isDrawingMultiSelect
  } = useEditor();
  const multiSelectStartNodeIds = useRef<string[]>([]);

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

  if (!multiSelectBox) {
    return null;
  }

  return (
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
  );
};
