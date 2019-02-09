# Setup instructions for Pwny Race participants

## Summary

* [Install OBS](https://obsproject.com)
  - Setup your scene to stream your desktop/VM and optionally picure-in-picture camera
  - Setup the stream destination to stream.pwny.racing/participant with a key according to below
* [Install Teamspeak](https://www.teamspeak.com/en/downloads/)
  - Connect to voice.pwny.racing
  - (Optional) Disable whipserlist notification sound.

## RTMP Settings

1. Make up a random string, this is your STREAM_KEY
2. Get the participant password from an organizer, this is the CURRENT_PARTICIPANT_PASSWORD
3. Setup OBS to stream to an RTMP server with the following settings
4. Send your STREAM_KEY to an organizer

URL: rtmp://stream.pwny.racing/participant
Streaming key: STREAM_KEY?psk=CURRENT_PARTICIPANT_PASSWORD
