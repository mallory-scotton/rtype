/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Package Project" icon.
 */
export const PackageProject: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <g clipPath='url(#clip0_7_5978)'>
        <path
          fillRule='evenodd'
          clipRule='evenodd'
          d='M7.93 10.8001L0 5.6001V10.8001L7.93 16.0001V10.8001Z'
          fill={color}
        />
        <path opacity='0.5' d='M16 5.6001L9 10.8001V16.0001L16 10.8001V5.6001Z' fill={color} />
        <path
          fillRule='evenodd'
          clipRule='evenodd'
          d='M11.0001 1.35L8.12015 0L0.410156 4.38L3.36015 6.22L11.0001 1.35Z'
          fill={color}
        />
        <path
          fillRule='evenodd'
          clipRule='evenodd'
          d='M4.93945 7.06002L8.46945 9.36002L15.5294 4.24002L12.2894 2.02002L4.93945 7.06002Z'
          fill={color}
        />
      </g>
      <defs>
        <clipPath id='clip0_7_5978'>
          <rect width='16' height='16' fill={color} />
        </clipPath>
      </defs>
    </svg>
  );
};
