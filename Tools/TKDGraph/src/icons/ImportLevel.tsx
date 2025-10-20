/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Import Level" icon.
 */
export const ImportLevel: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <g clip-path='url(#clip0_7_5703)'>
        <path
          opacity='0.5'
          fill-rule='evenodd'
          clip-rule='evenodd'
          d='M15 15L13 11L16 15V16H0V15L3 11L1 15H15Z'
          fill={color}
        />
        <path d='M9 13L6.5 6L3 14H13L10.5 9L9 13Z' fill={color} />
        <path d='M13 0H11V4H8.5L12 9L15.5 4H13V0Z' fill={color} />
      </g>
      <defs>
        <clipPath id='clip0_7_5703'>
          <rect width='16' height='16' fill={color} />
        </clipPath>
      </defs>
    </svg>
  );
};
