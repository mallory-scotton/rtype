/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Open Project" icon.
 */
export const OpenProject: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <g clip-path='url(#clip0_16_11155)'>
        <path d='M4 5V3H8V0L14 4L8 8V5H4Z' fill={color} />
        <path
          fill-rule='evenodd'
          clip-rule='evenodd'
          d='M7.93 10.8001L0 5.6001V10.8001L7.93 16.0001V10.8001Z'
          fill={color}
        />
        <path opacity='0.5' d='M16 5.6001L9 10.8001V16.0001L16 10.8001V5.6001Z' fill={color} />
      </g>
      <defs>
        <clipPath id='clip0_16_11155'>
          <rect width='16' height='16' fill={color} />
        </clipPath>
      </defs>
    </svg>
  );
};
