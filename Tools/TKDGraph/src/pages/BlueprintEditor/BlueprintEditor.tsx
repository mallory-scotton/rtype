/** Dependencies */
import React from 'react';
import { Reference, Canvas, MutliSelect, Layer, ContextMenu } from '../../components';
import './BlueprintEditor.css';
import { useEditor } from '../../context';

/**
 * @brief Interface for Blueprint Editor props
 * @description This interface defines the structure of the props that the Blueprint Editor component expects to receive.
 */
interface BlueprintEditorProps {
  type: string;
}

/**
 * @brief The Blueprint Editor Page
 * @description This page hosts the main blueprint editor interface.
 */
export const BlueprintEditor: React.FC<BlueprintEditorProps> = ({ type }) => {
  const { contextMenuPosition } = useEditor();

  return (
    <div className='bue-render'>
      <div className='frame-header'></div>
      <div className='frame'>
        <Layer>
          <Reference />
          <Canvas>
            {contextMenuPosition && <ContextMenu />}
            <MutliSelect />
          </Canvas>
        </Layer>
        <div className='blueprint-type'>{type.toUpperCase()}</div>
      </div>
    </div>
  );
};
