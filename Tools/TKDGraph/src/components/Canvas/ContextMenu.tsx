/** Dependencies */
import React from 'react';
import './ContextMenu.css';
import type { NodeIconType, PinType } from '../../types';
import { useEditor } from '../../context';

/**
 * @brief Props for ContextMenu component
 * @description Defines the properties required to render the ContextMenu component.
 */
export interface ContextMenuProps {
  from?: PinType;
}

/**
 * @brief CategoryTree interface
 * @description Represents a hierarchical structure for categories in the context menu.
 */
interface CategoryTree {
  name: string;
  nodes: {
    name: string;
    templateID: string;
    tags?: string[];
    icon: NodeIconType;
  }[];
  children?: CategoryTree[];
}

/**
 * @brief Helper function to highlight matching text
 * @description Splits text and wraps matching parts in a span with highlight class
 * Supports multi-word search (each word highlighted independently)
 */
const highlightText = (text: string, search: string): React.ReactNode => {
  if (!search) {
    return text;
  }

  // Split search into individual words
  const searchWords = search
    .trim()
    .split(/\s+/)
    .filter((word) => word.length > 0);

  if (searchWords.length === 0) {
    return text;
  }

  // Create a map of all match positions
  interface Match {
    start: number;
    end: number;
  }

  const matches: Match[] = [];
  const lowerText = text.toLowerCase();

  searchWords.forEach((word) => {
    const lowerWord = word.toLowerCase();
    let index = lowerText.indexOf(lowerWord);

    while (index !== -1) {
      matches.push({ start: index, end: index + word.length });
      index = lowerText.indexOf(lowerWord, index + 1);
    }
  });

  // Sort matches by start position and merge overlapping matches
  matches.sort((a, b) => a.start - b.start);

  const mergedMatches: Match[] = [];
  matches.forEach((match) => {
    if (mergedMatches.length === 0) {
      mergedMatches.push(match);
    } else {
      const last = mergedMatches[mergedMatches.length - 1];
      if (match.start <= last.end) {
        // Overlapping or adjacent - merge them
        last.end = Math.max(last.end, match.end);
      } else {
        mergedMatches.push(match);
      }
    }
  });

  // Build the result with highlights
  const parts: React.ReactNode[] = [];
  let lastIndex = 0;

  mergedMatches.forEach((match, idx) => {
    // Add text before match
    if (match.start > lastIndex) {
      parts.push(text.substring(lastIndex, match.start));
    }
    // Add highlighted match
    parts.push(
      <span key={`${idx}-highlight`} className='ue-ctx-highlight'>
        {text.substring(match.start, match.end)}
      </span>
    );
    lastIndex = match.end;
  });

  // Add remaining text
  if (lastIndex < text.length) {
    parts.push(text.substring(lastIndex));
  }

  return <>{parts}</>;
};

/**
 * @brief Helper function to check if node matches search
 * @description Checks both name and tags for matches
 * Supports multi-word search (node matches if it contains ALL of the words)
 */
const nodeMatchesSearch = (node: { name: string; tags?: string[] }, search: string): boolean => {
  if (!search) return true;

  // Split search into individual words
  const searchWords = search
    .trim()
    .split(/\s+/)
    .filter((word) => word.length > 0);

  if (searchWords.length === 0) return true;

  const lowerName = node.name.toLowerCase();
  const lowerTags = node.tags?.map((tag) => tag.toLowerCase()) ?? [];

  // Check if ALL words match in name or tags
  return searchWords.every((word) => {
    const lowerWord = word.toLowerCase();

    // Check name first (primary)
    if (lowerName.includes(lowerWord)) {
      return true;
    }

    // Check tags (secondary)
    if (lowerTags.some((tag) => tag.includes(lowerWord))) {
      return true;
    }

    return false;
  });
};

/**
 * @brief CategoryItem component
 * @description Renders a single category with expandable children
 */
const CategoryItem: React.FC<{
  category: CategoryTree;
  level: number;
  searchValue: string;
}> = ({ category, level, searchValue }) => {
  const [isExpanded, setIsExpanded] = React.useState(false);
  const { addNodeToBlueprint, currentBlueprintIndex, nodeRegistry, contextMenuPosition, setContextMenuPosition } =
    useEditor();

  // Filter nodes and children based on search (name and tags)
  const filteredNodes = category.nodes.filter((node) => nodeMatchesSearch(node, searchValue));

  // Recursively check children for matches
  const checkChildHasMatches = (child: CategoryTree): boolean => {
    const hasMatchingNodes = child.nodes.some((node) => nodeMatchesSearch(node, searchValue));
    const hasMatchingChildren = child.children?.some(checkChildHasMatches) ?? false;
    return hasMatchingNodes || hasMatchingChildren;
  };

  const filteredChildren = category.children?.filter(checkChildHasMatches);

  const hasContent = filteredNodes.length > 0 || (filteredChildren && filteredChildren.length > 0);

  if (!hasContent) {
    return null;
  }

  // Auto-expand if searching
  const shouldExpand = searchValue.length > 0 || isExpanded;

  // Handle Click on Node
  const handleNodeClick = (nodeName: string) => {
    console.log('Adding node at position:', contextMenuPosition, nodeName);

    if (currentBlueprintIndex !== -1) {
      const entry = nodeRegistry.getTemplate(nodeName);

      if (entry) {
        addNodeToBlueprint(
          {
            template: entry.name,
            data: nodeRegistry.generateNodeDataFromTemplate(entry),
            position: contextMenuPosition || { x: 0, y: 0 }
          },
          currentBlueprintIndex
        );
      }
    }

    setContextMenuPosition(null);
  };

  return (
    <div className='ue-ctx-category' style={{ paddingLeft: `${level * 12}px` }}>
      <div className='ue-ctx-category-header' onClick={() => setIsExpanded(!isExpanded)}>
        <span className={`ue-ctx-arrow ${shouldExpand ? 'expanded' : ''}`}>▶</span>
        <span className='ue-ctx-category-name'>{category.name}</span>
      </div>

      {shouldExpand && (
        <div className='ue-ctx-category-content'>
          {filteredNodes.map((node, index) => (
            <div
              key={index}
              data-id={node.templateID}
              className='ue-ctx-node-item'
              onClick={() => handleNodeClick(node.templateID)}
            >
              <div className={`icon ${node.icon}`}></div>
              {highlightText(node.name, searchValue)}
            </div>
          ))}

          {filteredChildren?.map((child, index) => (
            <CategoryItem key={index} category={child} level={level + 1} searchValue={searchValue} />
          ))}
        </div>
      )}
    </div>
  );
};

/**
 * @brief ContextMenu component
 * @description Renders a context editor interface.
 */
export const ContextMenu: React.FC<ContextMenuProps> = ({ from }) => {
  const [searchValue, setSearchValue] = React.useState('');
  const { nodeRegistry, contextMenuPosition } = useEditor();

  if (contextMenuPosition === null) {
    return null;
  }

  const templates = nodeRegistry.getAllTemplates();
  const categories: CategoryTree[] = [];

  templates.forEach((template) => {
    const categoryPath = template.category.split('>');
    let currentLevel = categories;

    categoryPath.forEach((categoryName, index) => {
      let category = currentLevel.find((cat) => cat.name === categoryName);
      if (!category) {
        category = { name: categoryName, nodes: [] };
        currentLevel.push(category);
      }

      if (!template.public) {
        return;
      }

      if (index === categoryPath.length - 1) {
        category.nodes.push({
          name: template.name,
          templateID: template.id,
          tags: template.tags,
          icon: template.icon
        });
      } else {
        if (!category.children) {
          category.children = [];
        }
        currentLevel = category.children;
      }
    });
  });

  return (
    <div
      data-scrollable
      className='ue-context-menu'
      style={{
        top: contextMenuPosition ? contextMenuPosition.y : '50%',
        left: contextMenuPosition ? contextMenuPosition.x : '50%'
      }}
    >
      <div className='ue-ctx-from'>
        {from && <div className={from}></div>}
        <span>{from ? `Actions taking a ${from}` : 'All Actions for this Blueprint'}</span>
      </div>

      <input
        className='ue-ctx-input'
        type='text'
        placeholder='Search'
        value={searchValue}
        onChange={(e) => setSearchValue(e.target.value)}
        autoFocus={true}
      />

      <div className='ue-ctx-categories-container'>
        {categories.map((category, index) => (
          <CategoryItem key={index} category={category} level={0} searchValue={searchValue} />
        ))}
      </div>
    </div>
  );
};
