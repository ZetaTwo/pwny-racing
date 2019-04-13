# Setup instructions for Pwny Race participants

## Summary

* [Install OBS](https://obsproject.com)
  - Setup your scene to stream your desktop/VM and optionally picure-in-picture camera
  - Setup the stream destination to stream.pwny.racing/participant with a key according to below
* [Install Teamspeak](https://www.teamspeak.com/en/downloads/)
  - Connect to voice.pwny.racing
  - (Optional) Disable whipserlist notification sound.

## RTMP Settings

1. Get your PLAYER_INDEX and the current PARTICIPANT_PASSWORD from the organizers.
2. Make up a random string, this is your STREAM_KEY
3. Setup OBS to stream to an RTMP server with the following settings
4. Send your STREAM_KEY to an organizer

URL: rtmp://stream.pwny.racing/participant_${PLAYER_INDEX}
Streaming key: STREAM_KEY?psk=CURRENT_PARTICIPANT_PASSWORD

Example:  
Your PLAYER_INDEX is 3 and the PARTICIPANT_PASSWORD is "supersecretpassword".
You pick the STREAM_KEY "imal33th4xxor". Your settings will be:

URL: "rtmp://stream.pwny.racing/participant_3"
Streaming key: "imal33th4xxor?psk=supersecretpassword"

To verify that the stream is working, you can then assemble this into a full URL and open in an RTMP capable player such as VLC.
Play URL: rtmp://stream.pwny.racing/participant_3/imal33th4xxor?psk=supersecretpassword
