/** Dependencies */
import React from 'react';
import './Canvas.css';
import { useEditor } from '../../context';

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
  const { canvasTransform } = useEditor();

  return (
    <div
      className='canvas'
      style={{
        transform: `translate(${canvasTransform.translateX}px, ${canvasTransform.translateY}px) scale(${canvasTransform.scale})`
      }}
    >
      {children}
    </div>
  );
};
