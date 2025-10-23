/** Dependencies */
import { useState, useRef, useEffect } from 'react';
import type { NodeRegistry } from '../../utils';

/**
 * @brief Context Menu Props
 */
interface ContextMenuProps {
  position: { x: number; y: number };
  nodeRegistry: NodeRegistry;
  onSpawnNode: (templateId: string) => void;
  onClose: () => void;
}

/**
 * @brief Context Menu Component
 * @description Displays a searchable, categorized list of node templates
 */
export const ContextMenu: React.FC<ContextMenuProps> = ({ position, nodeRegistry, onSpawnNode, onClose }) => {
  const [searchQuery, setSearchQuery] = useState('');
  const contextMenuRef = useRef<HTMLDivElement>(null);

  // Close menu on Escape key
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.key === 'Escape') {
        event.preventDefault();
        onClose();
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [onClose]);

  // Handle node selection
  const handleSelectNode = (templateId: string, _: string) => {
    onSpawnNode(templateId);
  };

  // Get all templates and filter by search query
  const allTemplates = nodeRegistry.getAllTemplates();
  const filteredTemplates = allTemplates.filter((template) =>
    template.name.toLowerCase().includes(searchQuery.toLowerCase())
  );

  // Group by category
  const categories = new Map<string, typeof filteredTemplates>();
  filteredTemplates.forEach((template) => {
    const category = template.category || 'Other';
    if (!categories.has(category)) {
      categories.set(category, []);
    }
    categories.get(category)!.push(template);
  });

  // Sort categories alphabetically
  const sortedCategories = Array.from(categories.entries()).sort((a, b) => a[0].localeCompare(b[0]));

  return (
    <div
      ref={contextMenuRef}
      data-context-menu
      style={{
        position: 'fixed',
        left: position.x,
        top: position.y,
        backgroundColor: '#2d2d30',
        border: '1px solid #3e3e42',
        borderRadius: '4px',
        minWidth: '250px',
        maxHeight: '500px',
        display: 'flex',
        flexDirection: 'column',
        zIndex: 10000,
        boxShadow: '0 2px 8px rgba(0, 0, 0, 0.3)'
      }}
      onClick={(e) => e.stopPropagation()}
      onMouseDown={(e) => e.stopPropagation()}
    >
      {/* Header */}
      <div
        style={{
          padding: '8px 12px',
          fontSize: '11px',
          color: '#999',
          fontWeight: 'bold',
          borderBottom: '1px solid #3e3e42'
        }}
      >
        Add Node
      </div>

      {/* Search Bar */}
      <div style={{ padding: '8px 12px', borderBottom: '1px solid #3e3e42' }}>
        <input
          type='text'
          placeholder='Search nodes...'
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
          autoFocus
          style={{
            width: '100%',
            padding: '6px 8px',
            backgroundColor: '#1e1e1e',
            border: '1px solid #3e3e42',
            borderRadius: '3px',
            color: '#cccccc',
            fontSize: '13px',
            outline: 'none'
          }}
          onFocus={(e) => {
            e.target.style.borderColor = '#007acc';
          }}
          onBlur={(e) => {
            e.target.style.borderColor = '#3e3e42';
          }}
        />
      </div>

      {/* Scrollable Content */}
      <div
        style={{
          overflowY: 'auto',
          maxHeight: '400px',
          padding: '4px 0'
        }}
      >
        {sortedCategories.length === 0 ? (
          <div
            style={{
              padding: '12px',
              fontSize: '13px',
              color: '#888',
              textAlign: 'center'
            }}
          >
            No nodes found
          </div>
        ) : (
          sortedCategories.map(([category, templates]) => (
            <div key={category}>
              {/* Category Header */}
              <div
                style={{
                  padding: '8px 12px',
                  fontSize: '11px',
                  color: '#888',
                  fontWeight: 'bold',
                  textTransform: 'uppercase',
                  backgroundColor: '#252526',
                  position: 'sticky',
                  top: 0,
                  zIndex: 1
                }}
              >
                {category}
              </div>

              {/* Templates in this category */}
              {templates.map((template) => (
                <div
                  key={template.name}
                  style={{
                    padding: '8px 12px 8px 24px',
                    cursor: 'pointer',
                    fontSize: '13px',
                    color: '#cccccc',
                    transition: 'background-color 0.1s'
                  }}
                  onMouseEnter={(e) => {
                    e.currentTarget.style.backgroundColor = '#094771';
                  }}
                  onMouseLeave={(e) => {
                    e.currentTarget.style.backgroundColor = 'transparent';
                  }}
                  onMouseDown={(e) => {
                    e.preventDefault();
                    e.stopPropagation();
                    handleSelectNode(template.id, template.name);
                  }}
                >
                  {template.name}
                </div>
              ))}
            </div>
          ))
        )}
      </div>
    </div>
  );
};
