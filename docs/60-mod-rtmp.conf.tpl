rtmp {
    server {
        listen 1935;
        chunk_size 4000;
        notify_method get;

        application participant_0 {
            live on;
            on_publish http://localhost:1936/auth_participant;
            on_play http://localhost:1936/auth_participant;
            record off;
            #push rtmp://a.rtmp.youtube.com/live2/{{ youtube_stream_key_0 }};
            #push rtmp://b.rtmp.youtube.com/live2/{{ youtube_stream_key_0 }}?backup=1;
        }
        application participant_1 {
            live on;
            on_publish http://localhost:1936/auth_participant;
            on_play http://localhost:1936/auth_participant;
            record off;
            #push rtmp://a.rtmp.youtube.com/live2/{{ youtube_stream_key_1 }};
            #push rtmp://b.rtmp.youtube.com/live2/{{ youtube_stream_key_1 }}?backup=1;
        }
        application participant_2 {
            live on;
            on_publish http://localhost:1936/auth_participant;
            on_play http://localhost:1936/auth_participant;
            record off;
            #push rtmp://a.rtmp.youtube.com/live2/{{ youtube_stream_key_2 }};
            #push rtmp://b.rtmp.youtube.com/live2/{{ youtube_stream_key_2 }}?backup=1;
        }
        application participant_3 {
            live on;
            on_publish http://localhost:1936/auth_participant;
            on_play http://localhost:1936/auth_participant;
            record off;
            #push rtmp://a.rtmp.youtube.com/live2/{{ youtube_stream_key_3 }};
            #push rtmp://b.rtmp.youtube.com/live2/{{ youtube_stream_key_3 }}?backup=1;
        }
        application participant {
            live on;
            on_publish http://localhost:1936/auth_participant;
            on_play http://localhost:1936/auth_participant;
            record off;
        }

        application master {
            live on;
            on_publish http://localhost:1936/auth_master;
            record off;
            push rtmp://a.rtmp.youtube.com/live2/{{ youtube_stream_key_master }};
            push rtmp://b.rtmp.youtube.com/live2/{{ youtube_stream_key_master }}?backup=1;
        }
    }
}
