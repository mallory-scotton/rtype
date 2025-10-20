/** Dependencies */
import { useState, useEffect } from 'react';
import { BlueprintCanvas } from './BlueprintCanvas';
import { BlueprintExplorer } from './BlueprintExplorer';
import { PropertyDrawer } from './PropertyDrawer';
import { useEditor } from '../context';

/**
 * @brief Node Editor Component
 * @description This component serves as the main editor for creating and managing nodes within the blueprint canvas.
 */
export const NodeEditor: React.FC = () => {
  const { currentBlueprintIndex, blueprints, setCurrentBlueprintIndex } = useEditor();
  const [showSavedIndicator, setShowSavedIndicator] = useState(false);

  // Get current blueprint (might be undefined)
  const currentBlueprint = blueprints[currentBlueprintIndex];

  /**
   * @brief Show auto-save indicator when blueprint changes
   */
  useEffect(() => {
    if (currentBlueprintIndex >= 0 && currentBlueprint) {
      setShowSavedIndicator(true);
      const timer = setTimeout(() => {
        setShowSavedIndicator(false);
      }, 2000);
      return () => clearTimeout(timer);
    }
  }, [currentBlueprint, currentBlueprintIndex]);

  /**
   * @brief Handle exporting current blueprint to JSON file
   */
  const handleSaveBlueprint = () => {
    if (!currentBlueprint) return;

    const dataStr = JSON.stringify(currentBlueprint, null, 2);
    const dataUri = 'data:application/json;charset=utf-8,' + encodeURIComponent(dataStr);

    const exportFileDefaultName = `${currentBlueprint.className}.blueprint.json`;

    const linkElement = document.createElement('a');
    linkElement.setAttribute('href', dataUri);
    linkElement.setAttribute('download', exportFileDefaultName);
    linkElement.click();
  };

  // Show explorer if no blueprint is selected (after all hooks)
  if (currentBlueprintIndex === -1 || !currentBlueprint) {
    return <BlueprintExplorer />;
  }

  return (
    <div id='blueprint-render-playground'>
      <div className='bue-render'>
        <div className='frame-header'>
          <button
            className='frame-header__back-btn'
            onClick={() => setCurrentBlueprintIndex(-1)}
            title='Back to Blueprint Explorer'
          >
            ← Back
          </button>
          <span className='frame-header__blueprint-name'>{currentBlueprint.className}</span>
          {showSavedIndicator && <span className='frame-header__auto-save-indicator'>✓ Auto-saved</span>}
          <button className='frame-header__save-btn' onClick={handleSaveBlueprint} title='Save blueprint to JSON file'>
            💾 Save
          </button>
        </div>
        <div className='frame' style={{ left: '280px', right: '380px' }}>
          <BlueprintCanvas />
          <div className='blueprint-type'>{currentBlueprint.type}</div>
          <div className='panel'></div>
          <div className='overlay' style={{ display: 'none' }}>
            Use CTRL + Scroll to zoom
          </div>
        </div>
      </div>
      <PropertyDrawer isOpen={true} />
    </div>
  );
};
