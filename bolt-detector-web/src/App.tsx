import { useState } from 'react'

function App() {
  const [video, setVideo] = useState<File | null>(null);

  const handleVideoChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (file) {
      setVideo(file);
    }
  };

  return (
    <div style = {{ padding : '20px' }}>
      <h1>Bolt Detector</h1>
      <input 
        type="file" 
        accept="video/*" 
        onChange={handleVideoChange} 
        />

        {video && (
          <div style={{ marginTop: '20px' }}>
            <p><strong>Name:</strong> {video.name}</p>
            <p><strong>Size:</strong> {(video.size / (1024 / 1024)).toFixed(2)} MB</p>
            <p><strong>Type:</strong> {video.type}</p>
          </div>
        )}
    </div>
  );
}

export default App
