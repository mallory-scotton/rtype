/** Dependencies */
import React, { useRef } from 'react';
import { useEditor } from '../../context';

/**
 * @brief Layer component props
 * @description This interface defines the structure of the props that the Layer component expects to receive.
 */
interface LayerProps {
  children: React.ReactNode | React.ReactNode[];
}

/**
 * @brief Layer component
 * @description This component represents a layer in the canvas, used to group related elements.
 */
export const Layer: React.FC<LayerProps> = ({ children }) => {
  // Use Editor Context
  const {
    selectedNodeIds,
    setMultiSelectBox,
    isDrawingMultiSelect,
    multiSelectStartNodeIds,
    canvasRef,
    canvasTransform,
    setCanvasTransform,
    isPanning,
    setIsPanning
  } = useEditor();

  // State for managing the transform
  const isDraggingRef = useRef(false);

  /**
   * @brief Handle mouse wheel event for zooming
   */
  const handleWheel = (event: React.WheelEvent) => {
    event.preventDefault();

    // Ensure the canvas ref is available
    if (!canvasRef.current) return;

    // Ensure the wheel event is on the transformable area
    const target = event.target as HTMLElement;
    if (target.closest('[data-scrollable]')) return;

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

    // Update the transform state
    setCanvasTransform({
      translateX: newTranslateX,
      translateY: newTranslateY,
      scale: newScale
    });
  };

  /**
   * @brief Handle mouse down event for panning
   */
  const handleMouseDown = (event: React.MouseEvent) => {
    // Check if the target or its ancestors have allowed pan classes
    let target = event.target as HTMLElement;
    let canPan = ['canvas', 'layer', 'reference'].some((cls) => target.classList.contains(cls));

    if (!canPan) return;

    // Handle left click for multi-select or deselection
    if (event.button === 0) {
      if (!event.shiftKey && !event.ctrlKey) {
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

    // Start panning
    setIsPanning(true);

    // Store initial mouse position and transform
    const startX = event.clientX;
    const startY = event.clientY;
    const startTranslateX = canvasTransform.translateX;
    const startTranslateY = canvasTransform.translateY;

    // Mark as dragging
    isDraggingRef.current = true;

    /**
     * @brief Handle mouse up event to stop panning
     */
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

    /**
     * @brief Handle mouse up event to stop panning
     */
    const onMouseUp = () => {
      isDraggingRef.current = false;
      setIsPanning(false);
      window.removeEventListener('mousemove', onMouseMove);
      window.removeEventListener('mouseup', onMouseUp);
    };

    // Attach event listeners
    window.addEventListener('mousemove', onMouseMove);
    window.addEventListener('mouseup', onMouseUp);
  };

  return (
    <div
      className='layer'
      ref={canvasRef}
      style={{ cursor: isPanning ? 'grabbing' : 'default' }}
      onWheel={handleWheel}
      onMouseDown={handleMouseDown}
    >
      {children}
    </div>
  );
};
