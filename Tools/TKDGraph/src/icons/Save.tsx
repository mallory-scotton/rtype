/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Save" icon.
 */
export const Save: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <path opacity='0.5' d='M12 1V7H3V1H1V15H3V10H13V15H15V4L12 1Z' fill={color} />
      <path d='M13 10H3V15H13V10Z' fill={color} />
      <path d='M3 1V7H12V1H3ZM11 6H9V2H11V6Z' fill={color} />
    </svg>
  );
};
