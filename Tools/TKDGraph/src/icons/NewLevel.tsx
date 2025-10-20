/** Dependencies */
import React from 'react';

/**
 * @brief Icon Component
 * @description A React component that renders the "New Level" icon.
 */
export const NewLevel: React.FC = () => {
  return (
    <svg width='16' height='16' viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <g clip-path='url(#clip0_7_5707)'>
        <path
          opacity='0.5'
          fill-rule='evenodd'
          clip-rule='evenodd'
          d='M15 15L13 11L16 15V16H0V15L3 11L1 15H15Z'
          fill='white'
        />
        <path d='M9 13L6.5 6L3 14H13L10.5 9L9 13Z' fill='white' />
        <path fill-rule='evenodd' clip-rule='evenodd' d='M16 3H13V0H11V3H8V5H11V8H13V5H16V3Z' fill='white' />
      </g>
      <defs>
        <clipPath id='clip0_7_5707'>
          <rect width='16' height='16' fill='white' />
        </clipPath>
      </defs>
    </svg>
  );
};
