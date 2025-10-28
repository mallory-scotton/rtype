/** Dependencies */
import React from 'react';
import './Main.css';

/**
 * @brief Main layout component
 * @description A layout component that wraps its children within a main layout div.
 */
export const MainLayout: React.FC<React.PropsWithChildren<unknown>> = ({ children }) => {
  return <div className='ue-main-layout'>{children}</div>;
};
