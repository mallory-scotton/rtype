/** Dependencies */
import React from 'react';
import './Editor.css';

/**
 * @brief Props Interface
 * @description Interface defining the props for the EditorLayout component.
 */
export interface EditorLayoutProps {
  children?: React.ReactNode | React.ReactNode[];
  rightSidebarWidth?: number;
  leftSidebarWidth?: number;
}

/**
 * @brief Editor Layout Component
 * @description A React component that represents the editor layout.
 */
export const EditorLayout: React.FC<EditorLayoutProps> = ({
  children,
  leftSidebarWidth = 300,
  rightSidebarWidth = 500
}) => {
  return (
    <div
      className='ue-editor-layout'
      style={{ gridTemplateColumns: `${leftSidebarWidth}px 1fr ${rightSidebarWidth}px` }}
    >
      {children}
    </div>
  );
};
