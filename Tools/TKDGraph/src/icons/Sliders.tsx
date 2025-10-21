/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Sliders" icon.
 */
export const Sliders: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg xmlns='http://www.w3.org/2000/svg' width={width} height={height} viewBox='0 0 24 24' fill='none' role='img'>
      <path
        fillRule='evenodd'
        clipRule='evenodd'
        d='M5.99994 14L6.00018 20L4.00018 20.0001L3.99994 14.0001L5.99994 14Z'
        fill={color}
      ></path>
      <path
        fillRule='evenodd'
        clipRule='evenodd'
        d='M5.99994 4.00006V11.0001H3.99994V4.00006H5.99994Z'
        fill={color}
      ></path>
      <path fillRule='evenodd' clipRule='evenodd' d='M15 9.00006H9V7.00006H15V9.00006Z' fill={color}></path>
      <path fillRule='evenodd' clipRule='evenodd' d='M8 15.0001H2V13.0001H8V15.0001Z' fill={color}></path>
      <path fillRule='evenodd' clipRule='evenodd' d='M22 13.0001H16V11.0001H22V13.0001Z' fill={color}></path>
      <path
        fillRule='evenodd'
        clipRule='evenodd'
        d='M13 4.00012L12.9998 8.00012L10.9998 8L11 4L13 4.00012Z'
        fill={color}
      ></path>
      <path
        fillRule='evenodd'
        clipRule='evenodd'
        d='M13 11L13.0002 20L11.0002 20.0001L11 11.0001L13 11Z'
        fill={color}
      ></path>
      <path fillRule='evenodd' clipRule='evenodd' d='M20 4.00006L20 12.0001H18L18 4.00006H20Z' fill={color}></path>
      <path
        fillRule='evenodd'
        clipRule='evenodd'
        d='M20 15L20.0002 20L18.0002 20.0001L18 15.0001L20 15Z'
        fill={color}
      ></path>
    </svg>
  );
};
