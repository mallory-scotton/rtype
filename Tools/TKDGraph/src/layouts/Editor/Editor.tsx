/** Dependencies */
import React, { useState, useRef, useCallback } from 'react';
import './Editor.css';

/**
 * @brief Props Interface
 * @description Interface defining the props for the EditorLayout component.
 */
export interface EditorLayoutProps {
  children?: React.ReactNode | React.ReactNode[];
}

/**
 * @brief Editor Layout Component
 * @description A React component that represents the editor layout.
 */
export const EditorLayout: React.FC<EditorLayoutProps> = ({ children }) => {
  // State for sidebar widths
  const [leftSidebarWidth, setLeftSidebarWidth] = useState(250);
  const [rightSidebarWidth, setRightSidebarWidth] = useState(500);

  // State to track which resizer (if any) is currently being dragged
  const [isDraggingLeft, setIsDraggingLeft] = useState(false);
  const [isDraggingRight, setIsDraggingRight] = useState(false);

  // Ref to store the initial mouse X position when a drag starts
  const dragStartX = useRef(0);

  /**
   * @brief Starts the drag operation for the left sidebar.
   * @param e MouseEvent
   */
  const startResizeLeft = (e: React.MouseEvent<HTMLDivElement>) => {
    // Prevent default behavior to avoid text selection during drag
    e.preventDefault();
    // Set dragging state and store the initial mouse X
    setIsDraggingLeft(true);
    dragStartX.current = e.clientX;
  };

  /**
   * @brief Starts the drag operation for the right sidebar.
   * @param e MouseEvent
   */
  const startResizeRight = (e: React.MouseEvent<HTMLDivElement>) => {
    e.preventDefault();
    setIsDraggingRight(true);
    dragStartX.current = e.clientX;
  };

  /**
   * @brief Handles mouse movement during a drag operation.
   * @param e MouseEvent
   */
  const handleMouseMove = useCallback(
    (e: React.MouseEvent<HTMLDivElement>) => {
      if (!isDraggingLeft && !isDraggingRight) return;

      const deltaX = e.clientX - dragStartX.current;

      // Resize Left Sidebar
      if (isDraggingLeft) {
        setLeftSidebarWidth((prevWidth) => {
          const newWidth = prevWidth + deltaX;
          // Basic boundary check (e.g., min width of 100px)
          return Math.max(100, newWidth);
        });
      }

      // Resize Right Sidebar
      else if (isDraggingRight) {
        setRightSidebarWidth((prevWidth) => {
          // For the right sidebar, a positive deltaX decreases the width
          const newWidth = prevWidth - deltaX;
          return Math.max(100, newWidth);
        });
      }

      // Update the starting X position for the next move event
      dragStartX.current = e.clientX;
    },
    [isDraggingLeft, isDraggingRight]
  );

  /**
   * @brief Stops the drag operation.
   */
  const stopResize = useCallback(() => {
    setIsDraggingLeft(false);
    setIsDraggingRight(false);
  }, []);

  /**
   * @brief Handle mouse down event on the main layout to potentially start resizing.
   */
  const handleMouseDown = (e: React.MouseEvent<HTMLDivElement>) => {
    // Determine if the mouse is near the left or right resizer areas
    const rect = e.currentTarget.getBoundingClientRect();
    const offsetX = e.clientX - rect.left;

    if (offsetX >= leftSidebarWidth - 4 && offsetX <= leftSidebarWidth) {
      startResizeLeft(e);
    } else if (offsetX >= rect.width - rightSidebarWidth && offsetX <= rect.width - rightSidebarWidth + 5) {
      startResizeRight(e);
    }
  };

  // Render the editor layout with adjustable sidebars
  return (
    <div
      className='ue-editor-layout'
      style={{
        gridTemplateColumns: `${leftSidebarWidth}px 1fr ${rightSidebarWidth}px`,
        cursor: isDraggingLeft || isDraggingRight ? 'ew-resize' : 'default'
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
