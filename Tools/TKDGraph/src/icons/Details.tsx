/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Details" icon.
 */
export const Details: React.FC<IconProps> = ({ width = 20, height = 20, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 20 20' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <path opacity='0.5' d='M6 19H19V18H8L6 19Z' fill={color} />
      <path opacity='0.5' d='M11 15H19V14H12L11 15Z' fill={color} />
      <path opacity='0.5' d='M15 11H19V10H16L15 11Z' fill={color} />
      <path opacity='0.5' d='M10 2H1V3H9L10 2Z' fill={color} />
      <path opacity='0.5' d='M6 6H1V7H5L6 6Z' fill={color} />
      <path
        d='M15.681 1.2188L17.8023 3.34012C17.9898 3.52766 18.0952 3.78195 18.0952 4.04716C18.0952 4.31238 17.9898 4.5668 17.8023 4.75434L7.25233 15.3043L3.7168 11.7688L14.2668 1.2188C14.4543 1.03126 14.7086 0.925781 14.9738 0.925781C15.239 0.925781 15.4934 1.03126 15.681 1.2188Z'
        fill={color}
      />
      <path d='M5.96 16.56L2.44 13.04L0 19L5.96 16.56Z' fill={color} />
    </svg>
  );
};
