/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Duplicate" icon.
 */
export const Duplicate: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg xmlns='http://www.w3.org/2000/svg' width={width} height={height} viewBox='0 0 24 24' fill='none' role='img'>
      <path
        d='M8.25 9C8.25 8.58579 8.58579 8.25 9 8.25H22C22.4142 8.25 22.75 8.58579 22.75 9V22C22.75 22.4142 22.4142 22.75 22 22.75H9C8.58579 22.75 8.25 22.4142 8.25 22V9Z'
        fill={color}
      ></path>
      <path
        d='M1.25 2C1.25 1.58579 1.58579 1.25 2 1.25H15C15.4142 1.25 15.75 1.58579 15.75 2V6.75H9C7.75736 6.75 6.75 7.75736 6.75 9V15.75H2C1.58579 15.75 1.25 15.4142 1.25 15V2Z'
        fill={color}
      ></path>
    </svg>
  );
};
