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
            #push rtmp://a.rtmp.youtube.com/live2/{{ participants[0].youtube_key }};
            #push rtmp://b.rtmp.youtube.com/live2/{{ participants[0].youtube_key }}?backup=1;
        }
        application participant_1 {
            live on;
            on_publish http://localhost:1936/auth_participant;
            on_play http://localhost:1936/auth_participant;
            record off;
            #push rtmp://a.rtmp.youtube.com/live2/{{ participants[1].youtube_key }};
            #push rtmp://b.rtmp.youtube.com/live2/{{ participants[1].youtube_key }}?backup=1;
        }
        application participant_2 {
            live on;
            on_publish http://localhost:1936/auth_participant;
            on_play http://localhost:1936/auth_participant;
            record off;
            #push rtmp://a.rtmp.youtube.com/live2/{{ participants[2].youtube_key }};
            #push rtmp://b.rtmp.youtube.com/live2/{{ participants[2].youtube_key }}?backup=1;
        }
        application participant_3 {
            live on;
            on_publish http://localhost:1936/auth_participant;
            on_play http://localhost:1936/auth_participant;
            record off;
            #push rtmp://a.rtmp.youtube.com/live2/{{ participants[3].youtube_key }};
            #push rtmp://b.rtmp.youtube.com/live2/{{ participants[3].youtube_key }}?backup=1;
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
            push rtmp://a.rtmp.youtube.com/live2/{{ episode.youtube_key }};
            push rtmp://b.rtmp.youtube.com/live2/{{ episode.youtube_key }}?backup=1;
        }
    }
}
