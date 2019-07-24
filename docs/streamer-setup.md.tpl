To: {{ participant_name }}  <{{ participant_email }}>
Subject: Setup instructions for Pwny Race participants

# Setup instructions for Pwny Race participants

These are the instructions to setup your computer for participating in the Pwny Racing.
This instruction set is intended for "{{ participant_name }}" in Pwny Racing episode {{ episode_number }}.

## Summary

* [Install OBS](https://obsproject.com)
  - Setup your scene to stream your desktop/VM and optionally picure-in-picture camera
  - Setup the stream destination to stream.pwny.racing/participant with a key according to below
* [Install Teamspeak](https://www.teamspeak.com/en/downloads/)
  - Connect to voice.pwny.racing
  - (Optional) Disable whipserlist notification sound.

## RTMP Settings

Setup OBS to stream to an RTMP server with the following settings

URL: rtmp://stream.pwny.racing/participant_{{ participant_index }}
Streaming key: {{ participant_key }}?psk={{ episode_password }}

To verify that the stream is working, you can then assemble this into a full URL and open in an RTMP capable player such as VLC.
Play URL: rtmp://stream.pwny.racing/participant_{{ participant_index }}/{{ participant_key }}?psk={{ episode_password }}

Greetings
Calle "Zeta Two" Svensson
and the rest of the Pwny Racing crew  

-------
