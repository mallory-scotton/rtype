/** Dependencies */
import React from 'react';
import type { CanvasTransform } from '../../types';
import './Reference.css';

/**
 * @brief Props for Reference component
 * @description Defines the properties required to render the Reference component.
 */
export interface ReferenceProps {
  transform: CanvasTransform;
  isPanning: boolean;
}

/**
 * @brief Reference component
 * @description Renders a reference area with specified transformations (translation and scaling).
 */
export const Reference: React.FC<ReferenceProps> = ({ transform }) => {
  return (
    <div
      className='reference'
      style={{
        transform: `translate(${transform.translateX}px, ${transform.translateY}px) scale(${transform.scale})`
      }}
    ></div>
  );
};
