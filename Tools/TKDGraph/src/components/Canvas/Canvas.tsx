/** Dependencies */
import React from 'react';
import type { CanvasTransform } from '../../types';
import './Canvas.css';

/**
 * @brief Props for Canvas component
 * @description Defines the properties required to render the Canvas component.
 */
export interface CanvasProps {
  transform: CanvasTransform;
  isPanning: boolean;
}

/**
 * @brief Canvas component
 * @description Renders a canvas area with specified transformations (translation and scaling).
 */
export const Canvas: React.FC<CanvasProps> = ({ transform }) => {
  return (
    <div
      className='canvas'
      style={{
        transform: `translate(${transform.translateX}px, ${transform.translateY}px) scale(${transform.scale})`
      }}
    ></div>
  );
};
