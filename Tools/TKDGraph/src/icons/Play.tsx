/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Play" icon.
 */
export const Play: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 20 20' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <path fill-rule='evenodd' clip-rule='evenodd' d='M4 1L18 10L4 19V1Z' fill={color} />
    </svg>
  );
};
