/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Texture" icon.
 */
export const Texture: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <path d='M5 2H2V5H5V2Z' fill={color} />
      <path d='M5 8H2V11H5V8Z' fill={color} />
      <path d='M8 5H5V8H8V5Z' fill={color} />
      <path d='M8 11H5V14H8V11Z' fill={color} />
      <path opacity='0.5' d='M15 10L10 15V14V10H15Z' fill={color} />
      <path d='M14 5H11V8H14V5Z' fill={color} />
      <path d='M11 2H8V5H11V2Z' fill={color} />
      <path opacity='0.5' d='M14 2V10H15V1H1V15H10V14H2V2H14Z' fill={color} />
      <path d='M10 14V10H14L15 9H9V15L10 14Z' fill={color} />
    </svg>
  );
};
