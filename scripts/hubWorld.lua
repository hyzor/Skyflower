function cutscene_intro()
	Print("intro")
	SetCamera(0, 0, 0, 0 ,45)
	AddPoint(-80, 80, -30 ,45 ,0, 0.5)
end


function loaded()
	if CompletedLevelCount() == 1 then
		skyflowerID = 10
		StartMorph(skyflowerID, 1,0,0,0.2)
		CutScenePlay("intro")
	end	
	
	-- Load correct signs in the hubworld, depending on if the subworlds are completed or not. --
	
	-- 4 = ID of subworld 1 -- 	
	-- 536 = ID of the sign that indicates that the player hasn't finished subworld 1 --
	-- 537 = ID of the sign that indicates that the player has finished subworld 1 --
	if LevelIsCompleted(4) then
		SetActivated(536, false)
		SetActivated(537, true)
	else
		SetActivated(536, true)
		SetActivated(537, false)
	end
	
	-- 5 = ID of subworld 2 --
	-- 538 = ID of the sign that indicates that the player hasn't finished subworld 2 --
	-- 539 = ID of the sign that indicates that the player has finished subworld 2 --
	if LevelIsCompleted(5) then
		SetActivated(538, false)
		SetActivated(539, true)
	else
		SetActivated(538, false)
		SetActivated(539, true)
	end
end

function activated_Level(id)
	if id == 33 then
		ChangeLevel(4)
	elseif id == 34 then
		ChangeLevel(5)
	end
end

-- Script controlling the particle system for the animating skyflower --

-- Particle textures -- 
-- 1 = flare1 --
-- 2 = flare2 --
-- 3 = particle --
-- 4 = bird --
-- 5 = dust --
-- 6 = flare1 --
-- 7 = portal --
-- 8 = aim --

function lerp(a, b, c)
	return ( a + (b - a) * c )
end

-- Variables --
-- TODO: Vec3?? --
xPos = 0.0 yPos = 0.0 zPos = 0.0
xAcc = 0.0 yAcc = 0.0 zAcc = 0.0
xDir = 0.0 yDir = 0.0 zDir = 0.0
xRandomVel = 0.0 yRandomVel = 0.0 zRandomVel = 0.0

emitFrequency = 0.0
ageLimit = 0.0
fadeTime = 0.0
timePassed = 0.0
offsetMaxXZ = 5.5
offsetMinXZ = -5.5
xTravelDir = 1.0
zTravelDir = 1.0
angle = 1.0
yFactor = 0.01
yFactor1 = 0.01
yFactor2 = 0.01
useRandomVelocity = false

-- Set starting values --
function start_skyflowerparticles()
	SetEmitPos(10, 0, 0.0, 0.0, 0.0)
	SetAcceleration(10, 0, 1.0, 1.0, 1.0)
	SetDirection(10, 0, 1.0, 1.0, 1.0)
	SetRandomVelocity(10, 0, 20.0, 5.0, 20.0)
	SetEmitFrequency(10, 0, 0.1)
	SetAgeLimit(10, 0, 2.5)
	SetFadeTime(10, 0, 1.5)
	SetRandomVelocityActive(10, 0, true)
	SetParticleType(10, 0, 3)
	SetScale(10, 0, 5.0, 5.0)
	SetFadeLimit(10, 0, 2.5)
	Activate(10, 0)
end

-- Reset variables to initial values --
function stop_skyflowerparticles()
	xPos = 0.0 yPos = 0.0 zPos = 0.0
	xAcc = 0.0 yAcc = 0.0 zAcc = 0.0
	xDir = 0.0 yDir = 0.0 zDir = 0.0
	xRandomVel = 0.0 yRandomVel = 0.0 zRandomVel = 0.0

	emitFrequency = 0.0
	ageLimit = 0.0
	fadeTime = 0.0
	timePassed = 0.0
	
	SetRandomVelocityActive(10, 0, false)
	Deactivate(10, 0)
end

function update_skyflowerparticles(dt)
	timePassed = timePassed + dt
	angle = angle + dt * 5
	
	xPos = xPos + (xTravelDir * dt * 20)
	zPos = zPos + (zTravelDir * dt * 20)
	yPos = yPos + (dt * 25)
	
	if (xPos > offsetMaxXZ) or (xPos < offsetMinXZ) then
		xTravelDir = xTravelDir * -1.0
	end
	
	if (zPos > offsetMaxXZ) or (zPos < offsetMinXZ) then
		zTravelDir = zTravelDir * -1.0
	end
	
	if(yPos > 20.0 and yPos < 40.0) then
		yFactor1 = yFactor1 + (dt * 0.25)
		SetEmitFrequency(10, 0, 0.05 * (0.1 / yFactor1))
	end
	
	if(yPos > 40.0 and yPos < 70.0) then
		yFactor2 = yFactor2 + (dt * 0.25)
		SetEmitFrequency(10, 0, 0.01 * (0.1 / yFactor2))
	end
	
	if(yPos > 70.0) then
		yFactor = yFactor + (dt * 0.75)
		SetEmitFrequency(10, 0, 0.001 * (0.1 / yFactor))
		--SetDirection(10, 0, math.cos(angle), -1.0, math.sin(angle))
		--SetAcceleration(10, 0, math.cos(angle), -10.0, math.sin(angle))
		SetRandomVelocity(10, 0, 100.0 * yFactor, 100.0 * yFactor, 100.0 * yFactor)
	end
	
	if(yPos < 70.0) then
		SetDirection(10, 0, math.cos(angle), -1.0, math.sin(angle))
		SetAcceleration(10, 0, math.cos(angle) * 10.0, -2.5, math.sin(angle) * 10.0)
	end
	
	SetEmitPos(10, 0, xPos, yPos, zPos)
end
























