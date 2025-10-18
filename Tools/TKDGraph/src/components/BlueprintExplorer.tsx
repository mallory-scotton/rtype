/** Dependencies */
import { useContext, useState, useRef } from 'react';
import { EditorContext } from '../context';
import type { BlueprintData, BlueprintType } from '../types';

/**
 * @brief Blueprint Explorer Component
 * @description This component allows users to create, open, or import blueprints before accessing the canvas.
 */
export const BlueprintExplorer: React.FC = () => {
  const { blueprints, setCurrentBlueprintIndex, addBlueprint, removeBlueprint } = useContext(EditorContext);

  const [showNewBlueprintModal, setShowNewBlueprintModal] = useState(false);
  const [newBlueprintName, setNewBlueprintName] = useState('');
  const [newBlueprintType, setNewBlueprintType] = useState<BlueprintType>('AActor');
  const fileInputRef = useRef<HTMLInputElement>(null);

  const blueprintTypes: BlueprintType[] = [
    'AActor',
    'APawn',
    'AGameMode',
    'AHud',
    'UActorComponent',
    'AController',
    'APlayerController',
    'AAIController',
    'ANetworkController'
  ];

  /**
   * @brief Handle creating a new blueprint
   */
  const handleCreateBlueprint = () => {
    if (!newBlueprintName.trim()) {
      alert('Please enter a blueprint name');
      return;
    }

    const newBlueprint: BlueprintData = {
      className: newBlueprintName,
      nodes: [],
      connections: [],
      type: newBlueprintType,
      properties: [],
      functions: []
    };

    addBlueprint(newBlueprint);
    setShowNewBlueprintModal(false);
    setNewBlueprintName('');
    setNewBlueprintType('AActor');
  };

  /**
   * @brief Handle opening an existing blueprint
   */
  const handleOpenBlueprint = (index: number) => {
    setCurrentBlueprintIndex(index);
  };

  /**
   * @brief Handle importing a blueprint from a file
   */
  const handleImportBlueprint = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (e) => {
      try {
        const content = e.target?.result as string;
        const blueprint: BlueprintData = JSON.parse(content);

        // Validate blueprint structure
        if (!blueprint.className || !blueprint.type || !blueprint.nodes || !blueprint.connections) {
          throw new Error('Invalid blueprint format');
        }

        addBlueprint(blueprint);
      } catch (error) {
        alert('Failed to import blueprint: ' + (error as Error).message);
      }
    };
    reader.readAsText(file);

    // Reset file input
    if (fileInputRef.current) {
      fileInputRef.current.value = '';
    }
  };

  /**
   * @brief Handle deleting a blueprint
   */
  const handleDeleteBlueprint = (index: number, event: React.MouseEvent) => {
    event.stopPropagation();
    if (confirm(`Are you sure you want to delete "${blueprints[index].className}"?`)) {
      removeBlueprint(index);
    }
  };

  /**
   * @brief Handle exporting a blueprint to a file
   */
  const handleExportBlueprint = (index: number, event: React.MouseEvent) => {
    event.stopPropagation();
    const blueprint = blueprints[index];
    const dataStr = JSON.stringify(blueprint, null, 2);
    const dataUri = 'data:application/json;charset=utf-8,' + encodeURIComponent(dataStr);

    const exportFileDefaultName = `${blueprint.className}.blueprint.json`;

    const linkElement = document.createElement('a');
    linkElement.setAttribute('href', dataUri);
    linkElement.setAttribute('download', exportFileDefaultName);
    linkElement.click();
  };

  return (
    <div className='blueprint-explorer'>
      <div className='blueprint-explorer__header'>
        <h1>Blueprint Explorer</h1>
        <p>Create a new blueprint, open an existing one, or import from a file</p>
      </div>

      <div className='blueprint-explorer__actions'>
        <button
          className='blueprint-explorer__action-btn blueprint-explorer__action-btn--primary'
          onClick={() => setShowNewBlueprintModal(true)}
        >
          <span className='icon'>+</span>
          Create New Blueprint
        </button>

        <button className='blueprint-explorer__action-btn' onClick={() => fileInputRef.current?.click()}>
          <span className='icon'>📁</span>
          Import Blueprint
        </button>

        <input
          ref={fileInputRef}
          type='file'
          accept='.json,.blueprint.json'
          style={{ display: 'none' }}
          onChange={handleImportBlueprint}
        />
      </div>

      {blueprints.length === 0 ? (
        <div className='blueprint-explorer__empty'>
          <div className='empty-state'>
            <div className='empty-state__icon'>📋</div>
            <h3 className='empty-state__title'>No Blueprints Yet</h3>
            <p className='empty-state__text'>Get started by creating a new blueprint or importing an existing one</p>
          </div>
        </div>
      ) : (
        <div className='blueprint-explorer__list'>
          <h2>Recent Blueprints</h2>
          <div className='blueprint-explorer__grid'>
            {blueprints.map((blueprint, index) => (
              <div key={index} className='blueprint-card' onClick={() => handleOpenBlueprint(index)}>
                <div className='blueprint-card__header'>
                  <span className='blueprint-card__type'>{blueprint.type}</span>
                  <div className='blueprint-card__actions'>
                    <button
                      className='blueprint-card__action-btn'
                      onClick={(e) => handleExportBlueprint(index, e)}
                      title='Export blueprint'
                    >
                      ⬇️
                    </button>
                    <button
                      className='blueprint-card__action-btn blueprint-card__action-btn--danger'
                      onClick={(e) => handleDeleteBlueprint(index, e)}
                      title='Delete blueprint'
                    >
                      🗑️
                    </button>
                  </div>
                </div>
                <h3 className='blueprint-card__name'>{blueprint.className}</h3>
                <div className='blueprint-card__stats'>
                  <span>{blueprint.nodes.length} nodes</span>
                  <span>{blueprint.connections.length} connections</span>
                  <span>{blueprint.properties.length} properties</span>
                  <span>{blueprint.functions.length} functions</span>
                </div>
              </div>
            ))}
          </div>
        </div>
      )}

      {showNewBlueprintModal && (
        <div className='modal-overlay' onClick={() => setShowNewBlueprintModal(false)}>
          <div className='modal' onClick={(e) => e.stopPropagation()}>
            <div className='modal__header'>
              <h2>Create New Blueprint</h2>
              <button className='modal__close' onClick={() => setShowNewBlueprintModal(false)}>
                ×
              </button>
            </div>
            <div className='modal__content'>
              <div className='form-group'>
                <label htmlFor='blueprint-name'>Blueprint Name</label>
                <input
                  id='blueprint-name'
                  type='text'
                  value={newBlueprintName}
                  onChange={(e) => setNewBlueprintName(e.target.value)}
                  placeholder='e.g., MyActor, PlayerCharacter'
                  autoFocus
                  onKeyPress={(e) => {
                    if (e.key === 'Enter') {
                      handleCreateBlueprint();
                    }
                  }}
                />
              </div>
              <div className='form-group'>
                <label htmlFor='blueprint-type'>Blueprint Type</label>
                <select
                  id='blueprint-type'
                  value={newBlueprintType}
                  onChange={(e) => setNewBlueprintType(e.target.value as BlueprintType)}
                >
                  {blueprintTypes.map((type) => (
                    <option key={type} value={type}>
                      {type}
                    </option>
                  ))}
                </select>
              </div>
            </div>
            <div className='modal__footer'>
              <button className='modal__btn modal__btn--secondary' onClick={() => setShowNewBlueprintModal(false)}>
                Cancel
              </button>
              <button className='modal__btn modal__btn--primary' onClick={handleCreateBlueprint}>
                Create
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
};
