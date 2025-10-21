import './App.css';
import { EditorProvider } from './context';
import { MainLayout, HeaderLayout, EditorLayout, VerticalLayout } from './layouts';
import TKDLogo from '../assets/icons/logo.png';
import { ActionBar, MainMenu } from './pages';
import { Tab } from './components/Tab';
import { Pawn } from './icons';

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
            <div style={{ display: 'flex', alignItems: 'center', gap: '2px' }}>
              <Tab title='BP_Player' selected={true} icon={<Pawn />} color='blueprint' />
              <Tab title='BP_PlayerController' icon={<Pawn />} color='blueprint' />
            </div>
            <ActionBar />
          </HeaderLayout>
          <EditorLayout>
            <VerticalLayout>
              <div style={{ backgroundColor: 'rgba(255, 0, 255, 0.2)' }}></div>
              <div style={{ backgroundColor: 'rgba(255, 255, 0, 0.2)' }}></div>
            </VerticalLayout>
            <div style={{ backgroundColor: 'rgba(0, 255, 0, 0.2)' }}></div>
            <div style={{ backgroundColor: 'rgba(0, 0, 255, 0.2)' }}></div>
          </EditorLayout>
        </MainLayout>
      </EditorProvider>
    </>
  );
}

export default App;
