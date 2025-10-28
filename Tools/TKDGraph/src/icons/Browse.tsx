/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Browse" icon.
 */
export const Browse: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <g clipPath='url(#clip0_7_11085)'>
        <path
          fillRule='evenodd'
          clipRule='evenodd'
          d='M9.6 13.2999L6.9 15.9999C6.9 15.9999 6.7 15.9999 6.3 15.6999C6 15.2999 6 15.0999 6 15.0999L8.7 12.3999L9.6 13.2999Z'
          fill={color}
        />
        <path
          d='M12 7.19995C13.5 7.19995 14.8 8.39995 14.8 9.99995C14.8 11.6 13.6 12.8 12 12.8C10.4 12.8 9.19995 11.5 9.19995 9.99995C9.19995 8.49995 10.5 7.19995 12 7.19995ZM12 6.19995C9.89995 6.19995 8.19995 7.89995 8.19995 9.99995C8.19995 12.1 9.89995 13.8 12 13.8C14.1 13.8 15.8 12.1 15.8 9.99995C15.8 7.89995 14.1 6.19995 12 6.19995Z'
          fill={color}
        />
        <path
          fillRule='evenodd'
          clipRule='evenodd'
          d='M0 13C0 13.6 0.4 14 1 14H5.1C5.7 13.5 6.4 12.8 7 12.1C6.7 11.4 6.6 10.6 6.6 9.8C6.8 7 9.3 4.7 12.1 4.7C12.7 4.7 13.2 4.8 13.7 5C14 5.1 14.5 5.4 15 5.7V5C15 4.4 14.6 4 14 4H0V13Z'
          fill={color}
        />
        <path
          opacity='0.5'
          fillRule='evenodd'
          clipRule='evenodd'
          d='M0 1C0 0.5 0.5 0 1 0H5.1C5.4 0 5.7 0.1 5.9 0.4L8.3 3H0V1Z'
          fill={color}
        />
      </g>
      <defs>
        <clipPath id='clip0_7_11085'>
          <rect width='16' height='16' fill={color} />
        </clipPath>
      </defs>
    </svg>
  );
};
