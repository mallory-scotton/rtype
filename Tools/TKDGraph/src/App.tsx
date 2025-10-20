import './App.css';
import { EditorProvider } from './context';
import { MainLayout, HeaderLayout } from './layouts';
import TKDLogo from '../assets/icons/logo.png';
import { MainMenu } from './pages';

/**
 * @brief Main application component that sets up the editor context and layout.
 * @returns The main application JSX element.
 */
function App() {
  return (
    <>
      <EditorProvider>
        <MainLayout>
          <HeaderLayout>
            <div>
              <img
                src={TKDLogo}
                alt='TKD Logo'
                style={{ marginLeft: '8px', marginTop: '7px', width: '52px', height: '52px', userSelect: 'none' }}
                draggable={false}
              />
            </div>
            <MainMenu />
            <div style={{ backgroundColor: 'rgba(0, 0, 255, .2)' }}></div>
            <div style={{ backgroundColor: '#242424' }}></div>
          </HeaderLayout>
        </MainLayout>
      </EditorProvider>
    </>
  );
}

export default App;
