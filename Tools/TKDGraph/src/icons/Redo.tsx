/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Redo" icon.
 */
export const Redo: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 20 20' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <path
        fillRule='evenodd'
        clipRule='evenodd'
        d='M12 1.16992L19 6.99992L12 12.8299V7.99989H7.17C6.5774 7.998 5.9903 8.11369 5.4427 8.34028C4.8952 8.56686 4.398 8.89982 3.98 9.31989C3.1397 10.165 2.6681 11.3082 2.6681 12.4999C2.6681 13.6916 3.1397 14.8349 3.98 15.6799C4.398 16.1 4.8952 16.4329 5.4427 16.6595C5.9903 16.8861 6.5774 17.0018 7.17 16.9999H9C9.2652 16.9999 9.5196 17.1053 9.7071 17.2928C9.8946 17.4803 10 17.7347 10 17.9999C10 18.2651 9.8946 18.5195 9.7071 18.707C9.5196 18.8946 9.2652 18.9999 9 18.9999H7.17C6.3159 19.0015 5.47 18.8344 4.6806 18.5084C3.8912 18.1823 3.174 17.7037 2.57 17.0999C1.3536 15.878 0.6707 14.2241 0.6707 12.4999C0.6707 10.7758 1.3536 9.12178 2.57 7.89989C3.174 7.2961 3.8912 6.81746 4.6806 6.49142C5.47 6.16538 6.3159 5.99835 7.17 5.99989H12V1.16992Z'
        fill={color}
      />
    </svg>
  );
};
