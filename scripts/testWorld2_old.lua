player = 1 --player id



trapButtonDown = false



--aiPush script--
-----------------

function load_aiPush(id)
	Print("AI loaded")
	
	StartUpdate()
end

buttontime = 0
inRange = false
function update_aiPush(id, dt)
	

	if not trapButtonDown then
		inRange = false
	
		SetTarget(id, 64) --start button
		if IsTouching(id, 64) then
			Jump(id)
		end
		
		if IsActivated(64) then --if button is down
			trapButtonDown = true
		end
	else
		SetTarget(id, player) --hunt player
		
		--if InRange(id, player, 30) then
			inRange = true --attack with both ai
			
			if CanPush(id, player) then
				Push(id, player)
			end
		--else
			--inRange = false
		--end
		
		--reactivate button after 5 sec
		buttontime = buttontime + dt
		if buttontime > 5 then
			buttontime = 0
			trapButtonDown = false
			inRange = false
		end
	end
	
end

--aiPush2 script--
------------------

function load_aiPush2(id)
	Print("AI2 loaded")
	
	StartUpdate()
end

function update_aiPush2(id, dt)
	if not inRange then
		SetTarget(id, player, 50) --hunt player 10 behind
	else
		--push player
		SetTarget(id, player)
		
		if CanPush(id, player) then
			Push(id, player)
		end
		
	end
end





--btnTrap script--
------------------
function activated_btnTrap(id)

	--trap!
	--Spawn(36, 9) --spawn enemy(10) on spawnpoint(7)
	Spawn(37, 9) --spawn enemy(11) on spawnpoint(7)
end

function deactivated_btnTrap(id)
	--trapButtonDown = false
end



--btnBig script--
-----------------

function activated_btnBig(id)
	--PlaySound(id, "player/jump1.wav")
	Jump(3) --woodblock
	--trapButtonDown = true
end









function activated_btnLevelCompleted(id)
	
end

function deactivated_btnLevelCompleted(id)
	if Level() ~= 0 then
		ChangeLevel(0) --hubb värld
	else
		ChangeLevel(1)
	end
end



function activated_save(id)
	Print("Saved")
end

function deactivated_save(id)
	
end









