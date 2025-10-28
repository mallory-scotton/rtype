/** Dependencies */
import React from 'react';
import './Header.css';

/**
 * @brief Header layout component
 * @description A layout component that wraps its children within a div with the class 'ue-header-layout'.
 */
export const HeaderLayout: React.FC<React.PropsWithChildren<unknown>> = ({ children }) => {
  return <div className='ue-header-layout'>{children}</div>;
};
