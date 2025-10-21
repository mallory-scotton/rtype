/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Exit" icon.
 */
export const Exit: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <path fillRule='evenodd' clipRule='evenodd' d='M6 1H16V16H6L14 14V3L6 1Z' fill={color} />
      <path d='M1 7V10H6V13L11 8.5L6 4V7H1Z' fill={color} />
    </svg>
  );
};
