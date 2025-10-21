/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Blueprint Find" icon.
 */
export const BlueprintFind: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <g clipPath='url(#clip0_7_5565)'>
        <mask
          id='mask0_7_5565'
          style={{ maskType: 'luminance' }}
          maskUnits='userSpaceOnUse'
          x='0'
          y='0'
          width='16'
          height='16'
        >
          <path d='M16 0H0V16H16V0Z' fill={color} />
        </mask>
        <g mask='url(#mask0_7_5565)'>
          <path d='M12 4H10V6H12V4Z' fill={color} />
          <path d='M12 8H10V10H12V8Z' fill={color} />
          <path d='M7 6H5V8H7V6Z' fill={color} />
          <path opacity='0.5' d='M11 5V4H8V7H6V8H8V10H11V9H9V5H11Z' fill={color} />
          <path
            fillRule='evenodd'
            clipRule='evenodd'
            d='M5.2 12.0998L1.3 15.9998C0.97536 15.9494 0.68026 15.7822 0.47 15.5298C0.213046 15.3233 0.0448718 15.0264 0 14.6998L3.9 10.7998L5.2 12.0998Z'
            fill={color}
          />
          <path
            d='M9 1.5C10.0878 1.5 11.1512 1.82257 12.0556 2.42692C12.9601 3.03126 13.6651 3.89025 14.0813 4.89524C14.4976 5.90023 14.6065 7.0061 14.3943 8.073C14.1821 9.13989 13.6583 10.1199 12.8891 10.8891C12.1199 11.6583 11.1399 12.1821 10.073 12.3943C9.0061 12.6065 7.90024 12.4976 6.89524 12.0813C5.89025 11.6651 5.03127 10.9601 4.42692 10.0556C3.82257 9.15117 3.5 8.0878 3.5 7C3.50265 5.54212 4.08296 4.14471 5.11384 3.11383C6.14471 2.08295 7.54212 1.50264 9 1.5ZM9 0.5C7.71442 0.5 6.45772 0.881218 5.3888 1.59545C4.31988 2.30968 3.48676 3.32484 2.99479 4.51256C2.50282 5.70028 2.37409 7.00721 2.6249 8.26809C2.8757 9.52896 3.49477 10.6872 4.40381 11.5962C5.31285 12.5052 6.47104 13.1243 7.73192 13.3751C8.99279 13.6259 10.2997 13.4972 11.4874 13.0052C12.6752 12.5132 13.6903 11.6801 14.4046 10.6112C15.1188 9.54229 15.5 8.28558 15.5 7C15.5 6.14641 15.3319 5.30117 15.0052 4.51256C14.6786 3.72394 14.1998 3.00739 13.5962 2.40381C12.9926 1.80023 12.2761 1.32144 11.4874 0.994783C10.6988 0.668127 9.85359 0.5 9 0.5Z'
            fill={color}
          />
        </g>
      </g>
      <defs>
        <clipPath id='clip0_7_5565'>
          <rect width='16' height='16' fill={color} />
        </clipPath>
      </defs>
    </svg>
  );
};
