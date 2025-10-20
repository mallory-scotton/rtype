/** Dependencies */
import React from 'react';

/**
 * @brief Icon Component
 * @description A React component that renders the "New Project" icon.
 */
export const NewProject: React.FC = () => {
  return (
    <svg width='16' height='16' viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <g clip-path='url(#clip0_16_11151)'>
        <path d='M12 5H9V8H7V5H4V3H7V0H9V3H12V5Z' fill='white' />
        <path
          fill-rule='evenodd'
          clip-rule='evenodd'
          d='M7.93 10.8001L0 5.6001V10.8001L7.93 16.0001V10.8001Z'
          fill='white'
        />
        <path opacity='0.5' d='M16 5.6001L9 10.8001V16.0001L16 10.8001V5.6001Z' fill='white' />
      </g>
      <defs>
        <clipPath id='clip0_16_11151'>
          <rect width='16' height='16' fill='white' />
        </clipPath>
      </defs>
    </svg>
  );
};
