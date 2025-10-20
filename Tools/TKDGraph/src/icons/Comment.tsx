/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Comment" icon.
 */
export const Comment: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <g clip-path='url(#clip0_7_5399)'>
        <path
          d='M8 0C3.6 0 0 2.7 0 6C0 8.4 1.9 10.5 4.7 11.4L2.5 16L7.6 12C7.7 12 7.9 12 8 12C12.4 12 16 9.3 16 6C16 2.7 12.4 0 8 0Z'
          fill={color}
        />
      </g>
      <defs>
        <clipPath id='clip0_7_5399'>
          <rect width='16' height='16' fill={color} />
        </clipPath>
      </defs>
    </svg>
  );
};
