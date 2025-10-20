/** Dependencies */
import React from 'react';
import './ActionBar.css';
import * as Icons from '../../icons';

/**
 * @brief ActionBar component
 * @description A React component for the ActionBar.
 */
export const ActionBar: React.FC = () => {
  return (
    <div className='ue-action-bar'>
      <div className='ue-action-bar-item'>
        <div className='ue-action-bar-item-button'>
          <div className='ue-action-bar-icon-overlay'>
            <Icons.Compile width={28} height={28} />
            <Icons.CompileStatusSuccess width={28} height={28} color='#8BC24A' />
          </div>
          <span>Compile</span>
        </div>
      </div>
      <div className='ue-action-bar-item'>
        <div className='ue-action-bar-item-button'>
          <Icons.Save width={28} height={28} />
          <span>Save</span>
        </div>
        <div className='ue-action-bar-item-button'>
          <Icons.Browse width={28} height={28} />
          <span>Browse</span>
        </div>
      </div>
      <div className='ue-action-bar-item'>
        <div className='ue-action-bar-item-button'>
          <Icons.BlueprintDiff width={28} height={28} />
          <span>Diff</span>
        </div>
      </div>
      <div className='ue-action-bar-item'>
        <div className='ue-action-bar-item-button'>
          <Icons.BlueprintFind width={28} height={28} />
          <span>Find</span>
        </div>
        <div className='ue-action-bar-item-button'>
          <Icons.HideUnrelated width={28} height={28} />
          <span>Hide Unrelated</span>
        </div>
      </div>
      <div className='ue-action-bar-item'>
        <div className='ue-action-bar-item-button'>
          <Icons.Settings width={28} height={28} />
          <span>Class Settings</span>
        </div>

        <div className='ue-action-bar-item-button'>
          <Icons.Save width={28} height={28} />
          <span>Class Defaults</span>
        </div>

        <div className='ue-action-bar-item-button'>
          <Icons.Simulate width={28} height={28} />
          <span>Simulation</span>
        </div>
      </div>
      <div className='ue-action-bar-item'></div>
    </div>
  );
};
