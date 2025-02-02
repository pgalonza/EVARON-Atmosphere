FROM devkitpro/devkita64:20240916

WORKDIR /app/

COPY ./requirements.txt /opt/requirements.txt

RUN apt-get install -y --no-install-recommends jq curl zip git

RUN apt-get install -y --no-install-recommends python3-venv && \
python3 -m venv /opt/venv && \
. /opt//venv/bin/activate && \
pip install --no-cache-dir -r /opt/requirements.txt

RUN dkp-pacman -Syyu --noconfirm && dkp-pacman -S --needed --noconfirm \
switch-glm switch-libjpeg-turbo \
devkitarm-rules hactool
