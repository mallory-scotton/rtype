/** Dependencies */
import React, { useState, useRef } from 'react';
import { Reference, Canvas } from '../../components';
import type { CanvasTransform } from '../../types';
import './BlueprintEditor.css';

/**
 * @brief Interface for Blueprint Editor props
 * @description This interface defines the structure of the props that the Blueprint Editor component expects to receive.
 */
interface BlueprintEditorProps {
  type: string;
}

/**
 * @brief The Blueprint Editor Page
 * @description This page hosts the main blueprint editor interface.
 */
export const BlueprintEditor: React.FC<BlueprintEditorProps> = ({ type }) => {
  // State for managing the transform
  const [transform, setTransform] = useState<CanvasTransform>({ translateX: 0, translateY: 0, scale: 1 });
  const canvasRef = useRef<HTMLDivElement>(null);
  const [isPanning, setIsPanning] = useState(false);
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
    const newScale = Math.min(Math.max(0.1, transform.scale + delta), 4);

    // Calculate the world position of the mouse before scaling
    const worldX = (mouseX - transform.translateX) / transform.scale;
    const worldY = (mouseY - transform.translateY) / transform.scale;

    // Calculate new translation to keep the mouse position fixed
    const newTranslateX = mouseX - worldX * newScale;
    const newTranslateY = mouseY - worldY * newScale;

    // Update the transform state
    setTransform({
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

    // Only handle middle mouse button or space + left click for panning
    if (event.button !== 1 && !(event.button === 0 && event.shiftKey)) {
      return;
    }

    // Start panning
    setIsPanning(true);

    // Store initial mouse position and transform
    const startX = event.clientX;
    const startY = event.clientY;
    const startTranslateX = transform.translateX;
    const startTranslateY = transform.translateY;

    // Mark as dragging
    isDraggingRef.current = true;

    /**
     * @brief Handle mouse up event to stop panning
     */
    const onMouseMove = (moveEvent: MouseEvent) => {
      if (!isDraggingRef.current) return;

      const deltaX = moveEvent.clientX - startX;
      const deltaY = moveEvent.clientY - startY;
      setTransform({
        ...transform,
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
    <div className='bue-render'>
      <div className='frame-header'></div>
      <div className='frame'>
        <div
          className='layer'
          ref={canvasRef}
          style={{ cursor: isPanning ? 'grabbing' : 'default' }}
          onWheel={handleWheel}
          onMouseDown={handleMouseDown}
        >
          <Reference transform={transform} isPanning={isPanning} />
          <Canvas transform={transform} isPanning={isPanning} />
        </div>
        <div className='blueprint-type'>{type.toUpperCase()}</div>
      </div>
    </div>
  );
};
