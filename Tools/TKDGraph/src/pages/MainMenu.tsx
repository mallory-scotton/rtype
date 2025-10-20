/** Dependencies */
import React, { useEffect } from 'react';
import { Menu, MenuItem, MenuSelection, MenuDivider } from '../components/Menu';
import * as Icons from '../icons';

/**
 * @brief Main menu component for the application.
 * @returns The main menu JSX element.
 */
export const MainMenu: React.FC = () => {
  // State to track the currently selected menu item
  const [selected, setSelected] = React.useState<string | null>(null);

  // Deselect menu item on mouse up anywhere in the document
  useEffect(() => {
    document.body.onmouseup = () => setSelected(null);

    return () => {
      document.body.onmouseup = null;
    };
  }, []);

  // Handler for mouse over event to select a menu item
  const handleMouseOver = (label: string) => {
    if (selected !== null) {
      setSelected(label);
    }
  };

  // Render the main menu with various menu items and selections
  return (
    <Menu>
      <MenuItem label='File' selected={selected === 'File'} onSelect={setSelected} onHover={handleMouseOver}>
        <MenuDivider label='OPEN' />
        <MenuSelection label='New level...' shortcut='CTRL+N' logo={<Icons.NewLevel />} />
        <MenuSelection label='Open a level...' shortcut='CTRL+O' logo={<Icons.OpenLevel />} />
        <MenuDivider label='' />
        <MenuSelection label='Open a ressource...' shortcut='CTRL+P' logo={<Icons.OpenAsset />} />
        <MenuDivider label='SAVE' />
        <MenuSelection label='Save' shortcut='CTRL+S' logo={<Icons.Save />} />
        <MenuSelection label='Save As...' shortcut='CTRL+ALT+S' logo={<Icons.SaveAs />} />
        <MenuSelection label='Save all' shortcut='CTRL+SHIFT+S' logo={<Icons.SaveAll />} />
        <MenuDivider label='IMPORT/EXPORT' />
        <MenuSelection label='Import into the level...' logo={<Icons.ImportLevel />} />
        <MenuSelection label='Export all...' logo={<Icons.Export />} />
        <MenuDivider label='PROJECT' />
        <MenuSelection label='New project...' logo={<Icons.NewProject />} />
        <MenuSelection label='Open a project...' logo={<Icons.OpenProject />} />
        <MenuSelection label='Compress project' logo={<Icons.PackageProject />} />
        <MenuSelection label='Recent projects' logo={<Icons.RecentProjects />} />
        <MenuDivider label='EXIT' />
        <MenuSelection label='Exit' logo={<Icons.Exit />} />
      </MenuItem>
      <MenuItem label='Edit' selected={selected === 'Edit'} onSelect={setSelected} onHover={handleMouseOver}>
        <MenuDivider label='UNDO/REDO' />
        <MenuSelection label='Undo' shortcut='CTRL+Z' logo={<Icons.Reset />} />
        <MenuSelection label='Redo' shortcut='CTRL+Y' logo={<Icons.Redo />} />
        <MenuDivider label='MODIFY' />
        <MenuSelection label='Cut' shortcut='CTRL+X' logo={<Icons.Copy />} />
        <MenuSelection label='Copy' shortcut='CTRL+C' logo={<Icons.Copy />} />
        <MenuSelection label='Paste' shortcut='CTRL+V' logo={<Icons.Paste />} />
        <MenuSelection label='Duplicate' shortcut='CTRL+D' logo={<Icons.Duplicate />} />
        <MenuSelection label='Delete' shortcut='DELETE' logo={<Icons.Delete />} />
        <MenuDivider label='CONFIGURATION' />
        <MenuSelection label='Editor preferences' logo={<Icons.Sliders />} />
        <MenuSelection label='Project settings' logo={<Icons.ProjectSettings />} />
      </MenuItem>
      <MenuItem label='Asset' selected={selected === 'Asset'} onSelect={setSelected} onHover={handleMouseOver} />
      <MenuItem label='View' selected={selected === 'View'} onSelect={setSelected} onHover={handleMouseOver} />
      <MenuItem label='Window' selected={selected === 'Window'} onSelect={setSelected} onHover={handleMouseOver} />
      <MenuItem label='Tools' selected={selected === 'Tools'} onSelect={setSelected} onHover={handleMouseOver} />
      <MenuItem label='Help' selected={selected === 'Help'} onSelect={setSelected} onHover={handleMouseOver}>
        <MenuDivider label='DOCUMENTATION' />
        <MenuSelection label='View editor documentation' shortcut='F1' logo={<Icons.Documentation />} />
        <MenuSelection label='View engine documentation' logo={<Icons.Documentation />} />
        <MenuSelection label='C++ API Reference documentation' logo={<Icons.Documentation />} />
        <MenuDivider label='COMMUNITY' />
        <MenuSelection label='Join the Discord server' logo={<Icons.Community />} />
        <MenuSelection label='Visit the GitHub repository' logo={<Icons.Blueprint />} />
        <MenuSelection label='Report an issue' logo={<Icons.Debug />} />
        <MenuSelection label='Request a feature' logo={<Icons.Comment />} />
        <MenuDivider label='SOFTWARE' />
        <MenuSelection label='About TKDGraph' logo={<Icons.Dirty />} />
        <MenuSelection label='Credits' logo={<Icons.Credits />} />
      </MenuItem>
    </Menu>
  );
};
