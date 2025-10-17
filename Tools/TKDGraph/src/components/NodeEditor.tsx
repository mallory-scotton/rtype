/** Dependencies */
import { BlueprintCanvas } from './BlueprintCanvas';

/**
 * @brief Node Editor Component
 * @description This component serves as the main editor for creating and managing nodes within the blueprint canvas.
 */
export const NodeEditor: React.FC = () => {
  return (
    <div id='blueprint-render-playground'>
      <div className='bue-render'>
        <div className='frame'>
          <BlueprintCanvas />
          <div className='frame-header'></div>
          <div className='blueprint-type'>BLUEPRINT</div>
          <div className='panel'></div>
          <div className='overlay' style={{ display: 'none' }}>
            Use CTRL + Scroll to zoom
          </div>
        </div>
      </div>
    </div>
  );
};
