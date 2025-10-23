/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Save All" icon.
 */
export const SaveAll: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <path opacity='0.5' d='M13 1V5H6V1H4V12H6V8H13V12H15V3L13 1Z' fill={color} />
      <path d='M13 8H6V12H13V8Z' fill={color} />
      <path d='M6 1V5H13V1H6ZM12 4H11V2H12V4Z' fill={color} />
      <path opacity='0.5' d='M3 13V3H1V15H13V13H3Z' fill={color} />
    </svg>
  );
};
