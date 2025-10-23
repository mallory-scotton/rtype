/** Dependencies */
import { useState } from 'react';
import { useEditor } from '../../context';
import { generatePropertyGetter, generatePropertySetter } from '../../constants/Property';
import type { PropertyEntry, PinType } from '../../types';

/**
 * @brief Property Drawer Component
 * @description UE5-style dual sidebar for managing class properties
 */
export const PropertyDrawer: React.FC<{ isOpen: boolean }> = ({ isOpen }) => {
  const { blueprints, currentBlueprintIndex, setBlueprints, nodeRegistry } = useEditor();
  const currentBlueprint = blueprints[currentBlueprintIndex];

  const [newProperty, setNewProperty] = useState<PropertyEntry>({
    name: '',
    type: 'int',
    value: 0
  });

  // Available property types (all PinTypes except 'exec')
  const propertyTypes: Exclude<PinType, 'exec'>[] = [
    'bool',
    'byte',
    'int',
    'float',
    'string',
    'name',
    'text',
    'real',
    'vector',
    'rotator',
    'transform',
    'struct',
    'object'
  ];

  /**
   * @brief Add a new property to the blueprint
   */
  const handleAddProperty = () => {
    if (!currentBlueprint || !newProperty.name.trim()) return;

    // Check for duplicate property names
    if (currentBlueprint.properties.some((p) => p.name === newProperty.name)) {
      alert('Property with this name already exists!');
      return;
    }

    const propertyToAdd: PropertyEntry = {
      ...newProperty,
      name: newProperty.name.trim()
    };

    // Register getter and setter nodes
    const getterTemplate = generatePropertyGetter(propertyToAdd);
    const setterTemplate = generatePropertySetter(propertyToAdd);

    nodeRegistry.registerNode(getterTemplate);
    nodeRegistry.registerNode(setterTemplate);

    // Update blueprint with new property
    setBlueprints(
      blueprints.map((bp, idx) =>
        idx === currentBlueprintIndex
          ? {
              ...bp,
              properties: [...bp.properties, propertyToAdd]
            }
          : bp
      )
    );

    // Reset form
    setNewProperty({
      name: '',
      type: 'int',
      value: getDefaultValueForType('int')
    });
  };

  /**
   * @brief Remove a property from the blueprint
   */
  const handleRemoveProperty = (propertyName: string) => {
    if (!currentBlueprint) return;

    const property = currentBlueprint.properties.find((p) => p.name === propertyName);
    if (!property) return;

    // Unregister getter and setter nodes
    nodeRegistry.unregisterNode(`get_${property.name}_${property.type}`);
    nodeRegistry.unregisterNode(`set_${property.name}_${property.type}`);

    // Update blueprint
    setBlueprints(
      blueprints.map((bp, idx) =>
        idx === currentBlueprintIndex
          ? {
              ...bp,
              properties: bp.properties.filter((p) => p.name !== propertyName)
            }
          : bp
      )
    );
  };

  /**
   * @brief Get default value for a property type
   */
  const getDefaultValueForType = (type: Exclude<PinType, 'exec'>): any => {
    switch (type) {
      case 'bool':
        return false;
      case 'byte':
      case 'int':
        return 0;
      case 'float':
      case 'real':
        return 0.0;
      case 'string':
      case 'name':
      case 'text':
        return '';
      case 'vector':
        return { x: 0, y: 0, z: 0 };
      case 'rotator':
        return { pitch: 0, yaw: 0, roll: 0 };
      case 'transform':
        return { location: { x: 0, y: 0, z: 0 }, rotation: { pitch: 0, yaw: 0, roll: 0 }, scale: { x: 1, y: 1, z: 1 } };
      case 'struct':
      case 'object':
        return null;
      default:
        return null;
    }
  };

  /**
   * @brief Handle property type change
   */
  const handleTypeChange = (type: Exclude<PinType, 'exec'>) => {
    setNewProperty({
      ...newProperty,
      type,
      value: getDefaultValueForType(type)
    });
  };

  if (!isOpen || !currentBlueprint) return null;

  return (
    <>
      {/* Left Sidebar - Property List */}
      <div className='property-sidebar-left'>
        <div className='property-sidebar-header'>
          <h3>Properties</h3>
          <span className='property-count'>{currentBlueprint.properties.length}</span>
        </div>

        <div className='property-sidebar-content'>
          {currentBlueprint.properties.length === 0 ? (
            <div className='property-empty-state'>No properties defined</div>
          ) : (
            <div className='property-list'>
              {currentBlueprint.properties.map((property) => (
                <div key={property.name} className='property-list-item'>
                  <div className='property-list-item-icon'>📋</div>
                  <div className='property-list-item-info'>
                    <div className='property-list-item-name'>{property.name}</div>
                    <div className='property-list-item-type'>{property.type}</div>
                  </div>
                  <button
                    className='property-list-item-delete'
                    onClick={() => handleRemoveProperty(property.name)}
                    title='Remove property'
                  >
                    ×
                  </button>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>

      {/* Right Sidebar - Details/Settings */}
      <div className='property-sidebar-right'>
        <div className='property-sidebar-header'>
          <h3>Details</h3>
        </div>

        <div className='property-sidebar-content'>
          {/* Add New Property Section */}
          <div className='property-section'>
            <h3>Add Property</h3>
            <div className='property-form'>
              <div className='property-form-group'>
                <label>Name</label>
                <input
                  type='text'
                  value={newProperty.name}
                  onChange={(e) => setNewProperty({ ...newProperty, name: e.target.value })}
                  placeholder='PropertyName'
                />
              </div>
              <div className='property-form-group'>
                <label>Type</label>
                <select
                  value={newProperty.type}
                  onChange={(e) => handleTypeChange(e.target.value as Exclude<PinType, 'exec'>)}
                >
                  {propertyTypes.map((type) => (
                    <option key={type} value={type}>
                      {type}
                    </option>
                  ))}
                </select>
              </div>
              <div className='property-form-group'>
                <label>Default Value</label>
                {newProperty.type === 'bool' ? (
                  <select
                    value={String(newProperty.value)}
                    onChange={(e) => setNewProperty({ ...newProperty, value: e.target.value === 'true' })}
                  >
                    <option value='false'>false</option>
                    <option value='true'>true</option>
                  </select>
                ) : ['int', 'byte', 'float', 'real'].includes(newProperty.type) ? (
                  <input
                    type='number'
                    value={newProperty.value}
                    onChange={(e) =>
                      setNewProperty({
                        ...newProperty,
                        value:
                          newProperty.type.includes('int') || newProperty.type === 'byte'
                            ? parseInt(e.target.value) || 0
                            : parseFloat(e.target.value) || 0
                      })
                    }
                    step={newProperty.type === 'float' || newProperty.type === 'real' ? '0.1' : '1'}
                  />
                ) : ['string', 'name', 'text'].includes(newProperty.type) ? (
                  <input
                    type='text'
                    value={newProperty.value}
                    onChange={(e) => setNewProperty({ ...newProperty, value: e.target.value })}
                  />
                ) : newProperty.type === 'vector' ? (
                  <div className='property-vector-inputs'>
                    <input
                      type='number'
                      placeholder='X'
                      value={newProperty.value?.x || 0}
                      onChange={(e) =>
                        setNewProperty({
                          ...newProperty,
                          value: { ...newProperty.value, x: parseFloat(e.target.value) || 0 }
                        })
                      }
                      step='0.1'
                    />
                    <input
                      type='number'
                      placeholder='Y'
                      value={newProperty.value?.y || 0}
                      onChange={(e) =>
                        setNewProperty({
                          ...newProperty,
                          value: { ...newProperty.value, y: parseFloat(e.target.value) || 0 }
                        })
                      }
                      step='0.1'
                    />
                    <input
                      type='number'
                      placeholder='Z'
                      value={newProperty.value?.z || 0}
                      onChange={(e) =>
                        setNewProperty({
                          ...newProperty,
                          value: { ...newProperty.value, z: parseFloat(e.target.value) || 0 }
                        })
                      }
                      step='0.1'
                    />
                  </div>
                ) : (
                  <input
                    type='text'
                    value={String(newProperty.value || '')}
                    onChange={(e) => setNewProperty({ ...newProperty, value: e.target.value })}
                    placeholder='Default value'
                  />
                )}
              </div>
              <button className='property-add-button' onClick={handleAddProperty} disabled={!newProperty.name.trim()}>
                + Add Property
              </button>
            </div>
          </div>
        </div>
      </div>
    </>
  );
};
