/** Dependencies */
import React, { useState, useRef, useCallback } from 'react';
import './Vertical.css';

/**
 * @brief Props Interface
 * @description Interface defining the props for the Vertical Layout component.
 */
export interface VerticalLayoutProps {
  children?: React.ReactNode | React.ReactNode[];
}

/**
 * @brief Vertical Layout Component
 * @description A React component that represents the vertical layout.
 */
export const VerticalLayout: React.FC<VerticalLayoutProps> = ({ children }) => {
  // State for layout heights
  const [dividerHeight, setDividerHeight] = useState(400);

  // State to track if the divider is being dragged
  const [isDragging, setIsDragging] = useState(false);

  // Ref to store the initial mouse Y position when a drag starts
  const dragStartY = useRef(0);

  /**
   * @brief Starts the drag operation for the divider.
   * @param e MouseEvent
   */
  const startResize = (e: React.MouseEvent<HTMLDivElement>) => {
    e.preventDefault();
    setIsDragging(true);
    dragStartY.current = e.clientY;
  };

  /**
   * @brief Handles mouse movement during a drag operation.
   * @param e MouseEvent
   */
  const handleMouseMove = useCallback(
    (e: React.MouseEvent<HTMLDivElement>) => {
      if (!isDragging) return;

      const deltaY = e.clientY - dragStartY.current;
      setDividerHeight((prevHeight) => Math.max(100, prevHeight + deltaY));
      dragStartY.current = e.clientY;
    },
    [isDragging]
  );

  /**
   * @brief Ends the drag operation.
   */
  const stopResize = useCallback(() => {
    setIsDragging(false);
  }, []);

  /**
   * @brief Handle mouse down event to initiate resizing.
   * @param e MouseEvent
   */
  const handleMouseDown = (e: React.MouseEvent<HTMLDivElement>) => {
    // Determine if the mouse is near the divider
    const rect = e.currentTarget.getBoundingClientRect();
    const offsetY = e.clientY - rect.top;

    // Ensure the mouse is within 10px of the divider to start resizing
    if (Math.abs(offsetY - dividerHeight) < 10) {
      startResize(e);
    }
  };

  return (
    <div
      className='ue-vertical-layout'
      style={{
        gridTemplateRows: `${dividerHeight}px 1fr`,
        cursor: isDragging ? 's-resize' : 'default'
      }}
      onMouseDown={handleMouseDown}
      onMouseMove={handleMouseMove}
      onMouseUp={stopResize}
      onMouseLeave={stopResize}
    >
      {children}
    </div>
  );
};
