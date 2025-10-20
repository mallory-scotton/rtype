/** Dependencies */
import React from 'react';

/**
 * @brief Icon Component
 * @description A React component that renders the "Open Level" icon.
 */
export const OpenLevel: React.FC = () => {
  return (
    <svg width='16' height='16' viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <g clip-path='url(#clip0_7_5711)'>
        <path
          opacity='0.5'
          fill-rule='evenodd'
          clip-rule='evenodd'
          d='M15 15L13 11L16 15V16H0V15L3 11L1 15H15Z'
          fill='white'
        />
        <path d='M9 13L6.5 6L3 14H13L10.5 9L9 13Z' fill='white' />
        <path d='M6 3V5H11V7.5L16 4L11 0.5V3H6Z' fill='white' />
      </g>
      <defs>
        <clipPath id='clip0_7_5711'>
          <rect width='16' height='16' fill='white' />
        </clipPath>
      </defs>
    </svg>
  );
};
