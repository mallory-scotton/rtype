/** Dependencies */
import React from 'react';
import './Reference.css';
import { useEditor } from '../../context';

/**
 * @brief Props for Reference component
 * @description Defines the properties required to render the Reference component.
 */
export interface ReferenceProps {}

/**
 * @brief Reference component
 * @description Renders a reference area with specified transformations (translation and scaling).
 */
export const Reference: React.FC<ReferenceProps> = ({}) => {
  const { canvasTransform } = useEditor();

  return (
    <div
      className='reference'
      style={{
        transform: `translate(${canvasTransform.translateX}px, ${canvasTransform.translateY}px) scale(${canvasTransform.scale})`
      }}
    ></div>
  );
};
